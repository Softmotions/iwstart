#include "iwstart.h"
#include "config.h"

#include <iowow/iwlog.h>
#include <iowow/iwp.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

struct env g_env = { 0 };

IW_NORET static void _usage(const char *err) {
  if (err) {
    fprintf(stderr, "\nError:\n\n");
    fprintf(stderr, "\t%s\n\n", err);
  }
  fprintf(stderr, "\n\tIOWOW/IWNET/EJDB2 Project boilerplate generaror\n");
  fprintf(stderr, "\nUsage %s [options]\n", g_env.program_file);
  fprintf(stderr, "\t-c, --conf=<>\t\t.ini configuration file\n");
  fprintf(stderr, "\t-V, --verbose\t\tPrint verbose output\n");
  fprintf(stderr, "\t-v, --version\t\tShow program version\n");
  fprintf(stderr, "\t-h, --help\t\tPrint usage help\n");
  fprintf(stderr, "\n");
  exit(err ? 1 : 0);
};

static void _on_signal(int signo) {
  if (g_env.verbose) {
    fprintf(stderr, "\nExiting on signal: %d\n", signo);
  }
  // iwn_poller_shutdown_request(g_env.poller);
  exit(0);
}

static void _destroy(void) {
  iwpool_destroy(g_env.pool);
}

static int _main(int argc, char *argv[]) {
  iwrc rc = 0;
  umask(0077);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
  signal(SIGALRM, SIG_IGN);
  signal(SIGUSR1, SIG_IGN);
  signal(SIGUSR2, SIG_IGN);

  if (  signal(SIGTERM, _on_signal) == SIG_ERR
     || signal(SIGINT, _on_signal) == SIG_ERR) {
    return EXIT_FAILURE;
  }
  RCC(rc, finish, iw_init());
  RCB(finish, g_env.pool = iwpool_create_empty());

  char *buf = getcwd(0, 0);
  if (!buf) {
    rc = iwrc_set_errno(IW_ERROR_ERRNO, errno);
    goto finish;
  }

  g_env.cwd = iwpool_strdup2(g_env.pool, buf), free(buf);
  RCB(finish, g_env.cwd);

  char path[PATH_MAX];
  RCC(rc, finish, iwp_exec_path(path, sizeof(path)));

  static const struct option long_options[] = {
    { "help",    0, 0, 'h' },
    { "verbose", 0, 0, 'V' },
    { "version", 0, 0, 'v' },
    { "conf",    1, 0, 'c' }
  };

  int ch;
  while ((ch = getopt_long(argc, argv, "c:hvV", long_options, 0)) != -1) {
    switch (ch) {
      case 'h':
        _usage(0);
        break;
      case 'v':
        // TODO: print version
        break;
      case 'V':
        g_env.verbose = true;
        break;
    }
  }


  finish:
  _destroy();
  if (rc) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

#ifdef IW_EXEC

int main(int argc, char *argv[]) {
  const char *v = VERSION_FULL;
  fprintf(stderr, "%s\n", v);
  //return _main(argc, argv);
  return 0;
}

#endif
