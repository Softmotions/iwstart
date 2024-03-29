#pragma once

#include <iowow/iwpool.h>

#define PROJECT_FLG_IOWOW         0x01U
#define PROJECT_FLG_IWNET         0x02U
#define PROJECT_FLG_EJDB2         0x04U
#define PROJECT_FLG_AWS4          0x08U
#define PROJECT_FLG_NO_UNCRUSTIFY 0x10U
#define PROJECT_FLG_NO_LVIMRC     0x20U
#define PROJECT_FLG_FORCE         0x40U


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
  const char *project_version;
  const char *project_author;
  const char *project_date;
  const char *project_website;
  uint32_t    project_flags;

  int  argc;
  bool verbose;
};

extern struct env g_env;
