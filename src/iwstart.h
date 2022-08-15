#pragma once

#include <iowow/iwpool.h>

#define PROJECT_FLG_IOWOW 0x01U
#define PROJECT_FLG_IWNET 0x02U
#define PROJECT_FLG_EJDB2 0x04U


struct env {
  const char  *cwd;
  const char  *program;
  const char  *program_file;
  const char  *config_file;
  const char  *config_file_dir;
  const char **argv;
  IWPOOL      *pool;

  const char *project_artifact;
  const char *project_name;
  const char *project_description;
  const char *project_license;
  const char *project_directory;
  const char *project_base_lib;
  const char *project_base_lib_cmake;
  const char *project_base_lib_static;
  uint32_t    project_flags;

  int  argc;
  bool verbose;
};

extern struct env g_env;
