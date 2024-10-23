#include "@project_artifact@.h"
#include "config.h"

#include <iowow/iwxstr.h>
#include <iowow/iwlog.h>
#include <iowow/iwp.h>
#include <iowow/iwutils.h>
#include <iowow/iwini.h>
#include <iowow/iwarr.h>

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <pthread.h>

struct env g_env = { 0 };

static pthread_mutex_t _mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t _barrier_term_signal_worker;
static pthread_t _thr_term_signal_worker = -1;
static struct iwulist _shutdown_hooks = { .usize = sizeof(void (*)(void)) };

/// Threads registry
static struct _thread {
  pthread_t t;
  bool      taken;
  struct _thread *next;
} *_threads;

static void _usage(const char *err) {
  if (err) {
    fprintf(stderr, "\nError:\n\n");
    fprintf(stderr, "\t%s\n\n", err);
  }
  fprintf(stderr, "\n\t@project_name@\n");
  fprintf(stderr, "\nUsage %s [options]\n", g_env.program);
  fprintf(stderr, "\t-c, --conf=<>          .ini configuration file\n");
  fprintf(stderr, "\t-V, --verbose          Print verbose output\n");
  fprintf(stderr, "\t-v, --version          Show program version\n");
  fprintf(stderr, "\t    --breakout-sigint  Enable gdb breaks on SIGINT.\n");
  fprintf(stderr, "\t-h, --help\t\tPrint usage help\n");
  fprintf(stderr, "\n");
};

void app_shutdown_request(void) {
  iwlog_warn2("@project_artifact@:: Shutdown requested");
  if (_thr_term_signal_worker != -1) {
    pthread_kill(_thr_term_signal_worker, SIGTERM);
  } else {
    exit(0);
  }
}

void app_shutdown_hook_register(void (*hook)(void)) {
  pthread_mutex_lock(&_mtx);
  iwulist_push(&_shutdown_hooks, &hook);
  pthread_mutex_unlock(&_mtx);
}

iwrc app_thread_register(pthread_t t) {
  iwrc rc = 0;
  struct _thread *node;
  RCB(finish, node = malloc(sizeof(*node)));
  pthread_mutex_lock(&_mtx);
  node->t = t;
  node->taken = false;
  node->next = _threads;
  _threads = node;
  pthread_mutex_unlock(&_mtx);
finish:
  return rc;
}

void app_thread_unregister_join(pthread_t t) {
  struct _thread *node = 0;
  bool take = false;
  pthread_mutex_lock(&_mtx);
  for (struct _thread *n = _threads, *p = 0; n; p = n, n = n->next) {
    if (n->t == t) {
      node = n;
      if (p) {
        p->next = n->next;
      } else {
        _threads = 0;
      }
      if (!node->taken) {
        node->taken = true;
        take = true;
      }
      break;
    }
  }
  pthread_mutex_unlock(&_mtx);
  if (take) {
    pthread_join(t, 0);
  }
  free(node);
}

void app_thread_unregister_detach(pthread_t t) {
  struct _thread *node = 0;
  bool take = false;
  pthread_mutex_lock(&_mtx);
  for (struct _thread *n = _threads, *p = 0; n; p = n, n = n->next) {
    if (n->t == t) {
      node = n;
      if (p) {
        p->next = n->next;
      } else {
        _threads = 0;
      }
      if (!node->taken) {
        node->taken = true;
        take = true;
      }
      break;
    }
  }
  pthread_mutex_unlock(&_mtx);
  if (take) {
    pthread_detach(t);
  }
  free(node);
}

struct _thread_run_spec {
  void (*worker)(void*);
  struct iwref_holder *ref;
  void *user_data;
};

static void* _thread_run(void *d) {
  struct _thread_run_spec *spec = d;
  spec->worker(spec->user_data);
  if (spec->ref) {
    iwref_unref(spec->ref);
  }
  free(spec);
  return 0;
}

iwrc app_thread_run_ref(struct iwref_holder *ref, void (*worker)(void*), void *user_data) {
  iwrc rc = 0;
  pthread_t th;

  struct _thread_run_spec *spec = malloc(sizeof(*spec));
  RCRA(spec);
  if (ref) {
    iwref_ref(ref);
  }
  spec->worker = worker;
  spec->ref = ref;
  spec->user_data = user_data;

  int rci = pthread_create(&th, 0, _thread_run, spec);
  if (rci) {
    rc = iwrc_set_errno(IW_ERROR_THREADING_ERRNO, rci);
    goto finish;
  }

  app_thread_register(th);

finish:
  if (rc) {
    if (ref) {
      iwref_unref(ref);
    }
    free(spec);
  }
  return rc;
}

iwrc app_thread_run(void (*worker)(void*), void *user_data) {
  return app_thread_run_ref(0, worker, user_data);
}

static void _shutdown_hooks_run(void) {
  pthread_mutex_lock(&_mtx);
  for (int i = iwulist_length(&_shutdown_hooks) - 1; i >= 0; --i) {
    void (*hook)(void) = *(void(**)(void))iwulist_get(&_shutdown_hooks, i);
    hook();
  }
  iwulist_destroy_keep(&_shutdown_hooks);
  pthread_mutex_unlock(&_mtx);
}

static int _ini_handler(void *user_data, const char *section, const char *name, const char *value) {
  iwrc rc = 0;
  iwlog_info("%s:%s=%s", section, name, value);
  if (!section || *section == '\0' || !strcmp(section, "main")) {
    if (!strcmp(name, "verbose")) {
      IWINI_PARSE_BOOL(g_env.verbose);
    }
  }
  return rc == 0;
}

static const char* _replace_config_env(const char *key, void *op) {
  if (!strcmp(key, "{home}")) {
    return getenv("HOME");
  } else if (!strcmp(key, "{cwd}")) {
    return g_env.cwd;
  }
  return 0;
}

static iwrc _config_load(void) {
  iwrc rc = 0;
  struct iwxstr *xstr = 0;
  char *data = 0;
  size_t len;

  RCB(finish, g_env.config_file_dir = iwpool_strdup2(g_env.pool, g_env.config_file));
  g_env.config_file_dir = iwp_dirname((char*) g_env.config_file_dir);

  data = iwu_file_read_as_buf_len(g_env.config_file, &len);
  if (!data) {
    rc = IW_ERROR_FAIL;
    iwlog_error("Error reading configuration file: %s", g_env.config_file);
    goto finish;
  }

  static const char *config_keys[] = { "{home}", "{cwd}" };
  RCC(rc, finish, iwu_replace(&xstr, data, len,
                              config_keys, sizeof(config_keys) / sizeof(config_keys[0]),
                              _replace_config_env, 0));

  rc = iwini_parse_string(iwxstr_ptr(xstr), _ini_handler, 0);

finish:
  free(data);
  iwxstr_destroy(xstr);
  return rc;
}

static void _shutdown(void) {
  static bool shutdown = false;
  if (__sync_bool_compare_and_swap(&shutdown, false, true)) {
    if (_thr_term_signal_worker != -1) {
      // Wait termination of signal handler thread
      pthread_kill(_thr_term_signal_worker, SIGUSR1);
      pthread_join(_thr_term_signal_worker, 0);
      _thr_term_signal_worker = -1;
    }

    _shutdown_hooks_run();

    {
      /// Join all registered threads
      int tn = 0;
      pthread_mutex_lock(&_mtx);
      for (struct _thread *n = _threads; n; n = n->next) {
        ++tn;
      }

      pthread_t jt[tn];
      tn = 0;
      for (struct _thread *n = _threads; n; n = n->next) {
        if (!n->taken) {
          n->taken = true;
          jt[tn++] = n->t;
        }
      }
      pthread_mutex_unlock(&_mtx);

      for (int i = 0; i < tn; ++i) {
        pthread_join(jt[i], 0);
      }
    }

    //CHECK:
    //iwn_poller_destroy(&g_env.poller);
  }
}

static void _deinit(void) {
  _shutdown();
  for (struct _thread *t = _threads, *n = 0; t; t = n) {
    n = t->next;
    free(t);
  }
  pthread_barrier_destroy(&_barrier_term_signal_worker);
  pthread_mutex_destroy(&_mtx);
  iwpool_destroy(g_env.pool);
}

IW_CONSTRUCTOR static void _init(void) {
  static bool init = false;
  if (__sync_bool_compare_and_swap(&init, false, true)) {
    umask(0077);
    if (!iw_init()) {
      abort();
    }
    g_env.pool = iwpool_create_empty();
    if (!g_env.pool) {
      abort();
    }
    if (pthread_barrier_init(&_barrier_term_signal_worker, 0, 2)) {
      abort();
    }
    atexit(_deinit);
  }
}

static void _breakout_sigint(void) {
  // In order to stop on Ctrl-C in GDB add breakpoint on line below
  // then run app with --breakout-sigint flag
  fprintf(stderr, "Breakout sigint\n");
}

static void _on_term_signal(int signo) {
  if (g_env.verbose) {
    fprintf(stderr, "\nExiting on signal: %d\n", signo);
  }
  _shutdown_hooks_run();
  // TODO: Call app termination staff like event loop exit, etc.
  // iwn_poller_shutdown_request(g_env.poller);
}

static void* _on_term_signal_worker(void *d) {
  int sig;
  sigset_t *ss = d;
  iwp_set_current_thread_name("@project_artifact@");
  pthread_barrier_wait(&_barrier_term_signal_worker);

retry:
  sig = 0, sigwait(ss, &sig);
  switch (sig) {
    case SIGINT:
      if (g_env.breakout_sigint) {
        _breakout_sigint();
        goto retry;
      }
    case SIGTERM:
      _on_term_signal(sig);
    case SIGUSR1:
      break;
    default:
      goto retry;
  }
  return 0;
}

iwrc run(void);

static int _main(int argc, char *argv[]) {
  int rv = EXIT_SUCCESS;
  iwrc rc = 0;
  sigset_t ss;

  if (prctl(PR_SET_PDEATHSIG, SIGKILL) == -1) {
    perror("Failed to set: prctl(PR_SET_PDEATHSIG, SIGKILL)");
    return -1;
  }

#ifndef NDEBUG
  if (prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY) == -1) {
    perror("Failed to se: prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY)");
    return -1;
  }
#endif

  _init();

  {
    sigemptyset(&ss);
    sigaddset(&ss, SIGTERM);
    sigaddset(&ss, SIGINT);
    sigaddset(&ss, SIGUSR1);
    sigaddset(&ss, SIGUSR2);
    sigaddset(&ss, SIGHUP);
    sigaddset(&ss, SIGALRM);
    sigaddset(&ss, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &ss, NULL);

    int rci = pthread_create(&_thr_term_signal_worker, 0, _on_term_signal_worker, &ss);
    if (rci) {
      iwlog_ecode_error2(iwrc_set_errno(IW_ERROR_THREADING_ERRNO, rci), "Error creating signal handling thread");
      return EXIT_FAILURE;
    }
    pthread_barrier_wait(&_barrier_term_signal_worker);
  }

  {
    char *buf = getcwd(0, 0);
    if (!buf) {
      rc = iwrc_set_errno(IW_ERROR_ERRNO, errno);
      goto finish;
    }
    g_env.cwd = iwpool_strdup2(g_env.pool, buf), free(buf);
    RCB(finish, g_env.cwd);
  }

  {
    char path[PATH_MAX + 1];
    RCC(rc, finish, iwp_exec_path(path, sizeof(path)));
    RCB(finish, g_env.program_file = iwpool_strdup2(g_env.pool, path));
    g_env.program = argc ? argv[0] : "";
  }

  g_env.argc = argc;
  RCB(finish, g_env.argv = iwpool_alloc(argc * sizeof(char*), g_env.pool));
  for (int i = 0; i < argc; ++i) {
    RCB(finish, g_env.argv[i] = iwpool_strdup2(g_env.pool, argv[i]));
  }

  static const struct option long_options[] = {
    { "help", 0, 0, 'h' },
    { "verbose", 0, 0, 'V' },
    { "version", 0, 0, 'v' },
    { "conf", 1, 0, 'c' },
    { "breakout-sigint", 0, 0, -2 },
    0
  };

  int ch;
  while ((ch = getopt_long(argc, argv, "c:hvV", long_options, 0)) != -1) {
    switch (ch) {
      case 'h':
        _usage(0);
        goto finish;
      case 'v':
        fprintf(stdout, "%s\n", VERSION_FULL);
        goto finish;
      case 'V':
        g_env.verbose = true;
        break;
      case 'c':
        g_env.config_file = iwpool_strdup2(g_env.pool, optarg);
        break;
      case -2:
        g_env.breakout_sigint = true;
        break;
      default:
        _usage(0);
        rv = EXIT_FAILURE;
        goto finish;
    }
  }


#ifdef NDEBUG
  if (!g_env.config_file) {
    char path[PATH_MAX + 1];
    snprintf(path, sizeof(path), "%s/@project_artifact@.ini", g_env.cwd);
    if (access(path, R_OK) == 0) {
      RCB(finish, g_env.config_file = iwpool_strdup2(g_env.pool, path));
    } else {
      const char *home = getenv("HOME");
      if (home) {
        snprintf(path, sizeof(path), "%s/.@project_artifact@/@project_artifact@.ini", home);
        if (access(path, R_OK) == 0) {
          RCB(finish, g_env.config_file = iwpool_strdup2(g_env.pool, path));
        }
      }
    }
  }
#endif

  if (g_env.config_file) {
    RCC(rc, finish, _config_load());
  }

  rc = run();

finish:
  _shutdown();
  if (rc) {
    return EXIT_FAILURE;
  }
  return rv;
}

#ifdef IW_EXEC

int main(int argc, char *argv[]) {
  return _main(argc, argv);
}

#endif
