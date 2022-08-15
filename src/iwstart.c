#include "iwstart.h"
#include "config.h"

#include <iowow/iwxstr.h>
#include <iowow/iwlog.h>
#include <iowow/iwp.h>
#include <iowow/iwutils.h>
#include <iowow/iwini.h>

#include <errno.h>
#include <ftw.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

struct env g_env = { 0 };

static void _destroy(void) {
  iwpool_destroy(g_env.pool);
}

static void _usage(const char *err) {
  if (err) {
    fprintf(stderr, "\n%s\n\n", err);
  }
  fprintf(stderr, "\n\tIOWOW/IWNET/EJDB2 Project boilerplate generator\n");
  fprintf(stderr, "\nUsage %s [options] <project directory>\n\n", g_env.program);
  fprintf(stderr, "\tNote: Options marked as * are required.\n\n");
  fprintf(stderr, "\t* -a, --artifact=<>\tProject main artifact name (required).\n");
  fprintf(stderr, "\t* -n, --name=<>\t\tShort project name.\n");
  fprintf(stderr, "\t  -b, --base=<>\t\tProject base lib. Either of: iowow,iwnet,ejdb2. Default: iwnet\n");
  fprintf(stderr, "\t  -d, --description=<>\tProject description text.\n");
  fprintf(stderr, "\t  -l, --license=<>\tProject license name.\n");
  fprintf(stderr, "\t  -u, --author=<>\tProject author.\n");
  fprintf(stderr, "\t  -w, --website=<>\tProject website.\n");
  fprintf(stderr, "\t      --no-uncrustify\tDisable uncrustify code form atter config\n");
  fprintf(stderr, "\t      --no-lvimrc\tDisable .lvimrc vim file generation\n");
  fprintf(stderr, "\t  -c, --conf=<>\t\t.ini configuration file.\n");
  fprintf(stderr, "\t  -V, --verbose\t\tPrint verbose output.\n");
  fprintf(stderr, "\t  -v, --version\t\tShow program version.\n");
  fprintf(stderr, "\t  -h, --help\t\tPrint usage help.\n");
  fprintf(stderr, "\n");
};

static bool _project_artifact_set(const char *val) {
  if (!val) {
    goto fail;
  }
  size_t len = strlen(val);
  if (!len) {
    goto fail;
  }
  for (size_t i = 0; i < len; ++i) {
    char c = val[i];
    if (i == 0 && (c >= '0' && c <= '9')) {
      goto fail;
    }
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')) {
      goto fail;
    }
  }
  g_env.project_artifact = iwpool_strdup2(g_env.pool, val);
  return g_env.project_artifact != 0;

fail:
  fprintf(stderr, "Invalid project artifact value. (-a, --artifact=<>)\n");
  return false;
}

static bool _project_base_lib_set(const char *val) {
  if (!val) {
    goto fail;
  }
  if (!strcmp(val, "iowow")) {
    g_env.project_flags |= PROJECT_FLG_IOWOW;
  } else if (!strcmp(val, "iwnet")) {
    g_env.project_flags |= PROJECT_FLG_IWNET;
  } else if (!strcmp(val, "ejdb2")) {
    g_env.project_flags |= PROJECT_FLG_EJDB2;
  }
  g_env.project_base_lib = iwpool_strdup2(g_env.pool, val);
  return g_env.project_base_lib != 0;

fail:
  fprintf(stderr, "Project base lib value must be one of: iowow,iwnet,ejdb. (-b, --base=<>)\n");
  return false;
}

static void _on_signal(int signo) {
  if (g_env.verbose) {
    fprintf(stderr, "\nExiting on signal: %d\n", signo);
  }
  // iwn_poller_shutdown_request(g_env.poller);
  exit(0);
}

static int _ini_handler(void *user_data, const char *section, const char *name, const char *value) {
  iwrc rc = 0;
  iwlog_info("%s:%s=%s", section, name, value);
  if (!section || !strcmp(section, "main")) {
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
  IWXSTR *xstr = 0;
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
  return rc;
}

static int _nfw_dir_yes(const char *fpath, const struct stat *sb, int tf, struct FTW *ftwbuf) {
  return (tf & FTW_D) == 0;
}

static bool _do_checks_dirs(void) {
  struct stat st;
  int rv = stat(g_env.project_directory, &st);
  if (rv == -1) {
    if (errno == ENOENT) {
      return true;
    } else {
      iwlog_ecode_error3(iwrc_set_errno(IW_ERROR_IO_ERRNO, errno));
      return false;
    }
  }
  if (!S_ISDIR(st.st_mode)) {
    fprintf(stderr, "%s is not a directory\n", g_env.project_directory);
    return false;
  }

  rv = nftw(g_env.project_directory, _nfw_dir_yes, 10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
  if (rv == -1) {
    iwlog_ecode_error3(iwrc_set_errno(IW_ERROR_IO_ERRNO, errno));
    return false;
  }
  if (rv) {
    fprintf(stderr, "Directory %s is not empty, aborting.\n", g_env.project_directory);
    return false;
  }
  return true;
}

iwrc iws_run(void);

static int _main(int argc, char *argv[]) {
  int rv = EXIT_SUCCESS;
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
    char path[PATH_MAX];
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
    { "help",          0, 0, 'h' },
    { "verbose",       0, 0, 'V' },
    { "version",       0, 0, 'v' },
    { "conf",          1, 0, 'c' },
    //
    { "artifact",      1, 0, 'a' },
    { "name",          1, 0, 'n' },
    { "base",          1, 0, 'b' },
    { "description",   1, 0, 'd' },
    { "license",       1, 0, 'l' },
    { "author",        1, 0, 'u' },
    { "website",       1, 0, 'w' },
    { "no-uncrustify", 0, 0, 'U' },
    { "no-lvimrc",     0, 0, 'L' },
  };

  int ch;
  while ((ch = getopt_long(argc, argv, "b:a:n:d:l:u:w:c:hvV", long_options, 0)) != -1) {
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
        RCB(finish, g_env.config_file = iwpool_strdup2(g_env.pool, optarg));
        break;
      case 'a':
        if (!_project_artifact_set(optarg)) {
          rv = EXIT_FAILURE;
          goto finish;
        }
        break;
      case 'n':
        RCB(finish, g_env.project_name = iwpool_strdup2(g_env.pool, optarg));
        break;
      case 'b':
        if (!_project_base_lib_set(optarg)) {
          rv = EXIT_FAILURE;
        }
        break;
      case 'd':
        RCB(finish, g_env.project_description = iwpool_strdup2(g_env.pool, optarg));
        break;
      case 'l':
        RCB(finish, g_env.project_license = iwpool_strdup2(g_env.pool, optarg));
        break;
      case 'w':
        RCB(finish, g_env.project_website = iwpool_strdup2(g_env.pool, optarg));
        break;
      case 'U':
        g_env.project_flags |= PROJECT_FLG_NO_UNCRUSTIFY;
        break;
      case 'L':
        g_env.project_flags |= PROJECT_FLG_NO_LVIMRC;
        break;
      default:
        _usage(0);
        rv = EXIT_FAILURE;
        goto finish;
    }
  }

  if (!g_env.project_directory) {
    if (argv[optind]) {
      RCB(finish, g_env.project_directory = iwpool_strdup2(g_env.pool, argv[optind]));
    } else {
      g_env.project_directory = g_env.cwd;
    }
  }

  if (g_env.config_file) {
    RCC(rc, finish, _config_load());
  }

  if (!g_env.project_artifact) {
    _usage("Missing required option: -a, --artifact=<>");
    rv = EXIT_FAILURE;
    goto finish;
  }
  if (!g_env.project_name) {
    _usage("Missing required option: -n, --name=<>");
    rv = EXIT_FAILURE;
    goto finish;
  }
  if (!g_env.project_base_lib && !_project_base_lib_set("iwnet")) {
    rv = EXIT_FAILURE;
    goto finish;
  }
  if (!g_env.project_license) {
    g_env.project_license = "Proprietary";
  }

  if (!_do_checks_dirs()) {
    rv = EXIT_FAILURE;
    goto finish;
  }

  if (!g_env.project_version) {
    g_env.project_version = "1.0.0";
  }

  if (!g_env.project_description) {
    g_env.project_description = "";
  }

  if (!g_env.project_website) {
    g_env.project_website = "";
  }

  if (!g_env.project_author) {
    if (getenv("DEBFULLNAME")) {
      IWXSTR *xstr = iwxstr_new();
      RCB(finish, xstr);
      RCC(rc, finish, iwxstr_cat2(xstr, getenv("DEBFULLNAME")));
      if (getenv("DEBEMAIL")) {
        RCC(rc, finish, iwxstr_printf(xstr, " <%s>", getenv("DEBEMAIL")));
      }
      RCB(finish, g_env.project_author = iwpool_strndup2(g_env.pool, iwxstr_ptr(xstr), iwxstr_size(xstr)));
      iwxstr_destroy(xstr);
    } else if (getenv("USER")) {
      g_env.project_author = getenv("USER");
    } else if (getenv("LOGNAME")) {
      g_env.project_author = getenv("LOGNAME");
    }
    if (!g_env.project_author) {
      g_env.project_author = "";
    }
  }

  if (!g_env.project_date) {
    time_t current;
    char rfc_2822[40];
    time(&current);
    strftime(
      rfc_2822,
      sizeof(rfc_2822),
      "%a, %d %b %Y %T %z",
      localtime(&current));
    RCB(finish, g_env.project_date = iwpool_strdup2(g_env.pool, rfc_2822));
  }


  if (g_env.project_flags & PROJECT_FLG_IOWOW) {
    g_env.project_base_lib_cmake = "AddIOWOW";
    g_env.project_base_lib_static = "IOWOW::static";
  } else if (g_env.project_flags & PROJECT_FLG_IWNET) {
    g_env.project_base_lib_cmake = "AddIWNET";
    g_env.project_base_lib_static = "IWNET::static";
  } else if (g_env.project_flags & PROJECT_FLG_EJDB2) {
    g_env.project_base_lib_cmake = "AddEJDB2";
    g_env.project_base_lib_static = "EJDB2::static";
  } else {
    rc = IW_ERROR_ASSERTION;
    goto finish;
  }

  RCC(rc, finish, iws_run());

  fprintf(stdout, "'%s' project created at %s\n", g_env.project_name, g_env.project_directory);

finish:
  _destroy();
  if (rc) {
    iwlog_ecode_error3(rc);
    return EXIT_FAILURE;
  }
  return rv;
}

#ifdef IW_EXEC

int main(int argc, char *argv[]) {
  return _main(argc, argv);
}

#endif
