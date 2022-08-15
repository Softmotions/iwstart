#include "iwstart.h"

#include "cmake_iowow_add.inc"
#include "cmake_iwnet_add.inc"
#include "cmake_ejdb2_add.inc"

#include "cmake_lists.inc"
#include "src_cmake_lists.inc"

#include "tests_cmake_lists.inc"
#include "test1.inc"

#include "tools_cmake_lists.inc"
#include "tools_strliteral.inc"

#include "app_h.inc"
#include "app_c.inc"
#include "app_run.inc"

#include "readme_md.inc"
#include "changelog.inc"
#include "uncrustify.inc"
#include "lvimrc.inc"
#include "gitignore.inc"
#include "license.inc"

#include <iowow/iwlog.h>
#include <iowow/iwp.h>
#include <iowow/iwhmap.h>
#include <iowow/iwxstr.h>
#include <iowow/iwutils.h>
#include <iowow/iwjson.h>

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

struct ctx {
  IWHMAP *keys;
};

static const char* _replace_key(const char *key, void *d) {
  struct ctx *ctx = d;
  size_t off = (uintptr_t) iwhmap_get(ctx->keys, key);
  if (off) {
    const char *pv;
    memcpy(&pv, (char*) &g_env + off, sizeof(const char*));
    return pv;
  } else if (!strcmp("{home}", key)) {
    return getenv("HOME");
  } else {
    return 0;
  }
}

static const char *_keys[] = {
  "{home}",
  "{cwd}",
  "{project_artifact}",
  "{project_name}",
  "{project_description}",
  "{project_license}",
  "{project_directory}",
  "{project_base_lib}",
  "{project_base_lib_cmake}",
  "{project_base_lib_static}",
  "{project_version}",
  "{project_author}",
  "{project_date}",
  "{project_website}"
};

static iwrc _replace(struct ctx *ctx, const char *data, size_t data_len, char **out) {
  iwrc rc = 0;
  *out = 0;
  IWXSTR *xstr = iwxstr_new();
  if (!xstr) {
    return iwrc_set_errno(IW_ERROR_ALLOC, errno);
  }
  RCC(rc, finish, iwu_replace(&xstr, data, data_len, _keys, sizeof(_keys) / sizeof(_keys[0]), _replace_key, ctx));
finish:
  if (rc) {
    iwlog_ecode_error3(rc);
    iwxstr_destroy(xstr);
  } else {
    *out = iwxstr_destroy_keep_ptr(xstr);
  }
  return rc;
}

static iwrc _install(
  struct ctx          *ctx,
  const unsigned char *data_,
  const unsigned int   data_len,
  const char          *path_,
  bool                 replace_data
  ) {
  iwrc rc = 0;
  char *path = 0, *data = 0;
  FILE *file = 0;
  IWXSTR *xstr = iwxstr_new();
  RCB(finish, xstr);
  RCC(rc, finish, _replace(ctx, path_, strlen(path_), &path));
  if (replace_data) {
    RCC(rc, finish, _replace(ctx, (char*) data_, data_len, &data));
  } else {
    data = (char*) data_;
  }
  RCC(rc, finish, iwxstr_printf(xstr, "%s/%s", g_env.project_directory, path));
  RCC(rc, finish, iwp_mkdirs_for_file(iwxstr_ptr(xstr)));

  file = fopen(iwxstr_ptr(xstr), "w+");
  if (!file) {
    rc = iwrc_set_errno(IW_ERROR_IO_ERRNO, errno);
    goto finish;
  }

  if (fwrite(data, strlen(data), 1, file) != 1) {
    fprintf(stderr, "Failed to write into: %s\n", path);
    rc = IW_ERROR_IO_ERRNO;
    goto finish;
  }

finish:
  iwxstr_destroy(xstr);
  if (path != path_) {
    free(path);
  }
  if (data != (char*) data_) {
    free(data);
  }
  if (file) {
    fclose(file);
  }
  return rc;
}

static iwrc _install_app_json(struct ctx *ctx) {
  iwrc rc = 0;
  IWXSTR *xstr = 0;
  JBL jbl;
  
  RCB(finish, xstr = iwxstr_new());
  RCC(rc, finish, jbl_create_empty_object(&jbl));
  RCC(rc, finish, jbl_set_string(jbl, "project_artifact", g_env.project_artifact));
  RCC(rc, finish, jbl_set_string(jbl, "project_author", g_env.project_author));
  RCC(rc, finish, jbl_set_string(jbl, "project_base_lib", g_env.project_base_lib));
  RCC(rc, finish, jbl_set_string(jbl, "project_base_lib_cmake", g_env.project_base_lib_cmake));
  RCC(rc, finish, jbl_set_string(jbl, "project_base_lib_static", g_env.project_base_lib_static));
  RCC(rc, finish, jbl_set_string(jbl, "project_date", g_env.project_date));
  RCC(rc, finish, jbl_set_string(jbl, "project_description", g_env.project_description));
  RCC(rc, finish, jbl_set_string(jbl, "project_license", g_env.project_license));
  RCC(rc, finish, jbl_set_string(jbl, "project_name", g_env.project_name));
  RCC(rc, finish, jbl_set_string(jbl, "project_website", g_env.project_website));

  RCC(rc, finish, jbl_as_json(jbl, jbl_xstr_json_printer, xstr, JBL_PRINT_PRETTY));

  rc = _install(ctx, (void*) iwxstr_ptr(xstr), iwxstr_size(xstr), ".app.json", false);

finish:
  iwxstr_destroy(xstr);
  jbl_destroy(&jbl);
  return rc;
}

iwrc iws_run(void) {
  iwrc rc = 0;
  struct ctx ctx = {
    .keys = iwhmap_create_str(0)
  };
  RCB(finish, ctx.keys);

  RCC(rc, finish, iwhmap_put(ctx.keys, "{cwd}", (void*) offsetof(struct env, cwd)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_artifact}", (void*) offsetof(struct env, project_artifact)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_name}", (void*) offsetof(struct env, project_name)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_description}", (void*) offsetof(struct env, project_description)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_license}", (void*) offsetof(struct env, project_license)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_directory}", (void*) offsetof(struct env, project_directory)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_base_lib}", (void*) offsetof(struct env, project_base_lib)));
  RCC(rc, finish,
      iwhmap_put(ctx.keys, "{project_base_lib_cmake}", (void*) offsetof(struct env, project_base_lib_cmake)));
  RCC(rc, finish,
      iwhmap_put(ctx.keys, "{project_base_lib_static}", (void*) offsetof(struct env, project_base_lib_static)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_version}", (void*) offsetof(struct env, project_version)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_author}", (void*) offsetof(struct env, project_author)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_date}", (void*) offsetof(struct env, project_date)));
  RCC(rc, finish, iwhmap_put(ctx.keys, "{project_website}", (void*) offsetof(struct env, project_website)));

#define _INSTALL(name__, replace_data__) \
  RCC(rc, finish, _install(&ctx, name__, name__ ## _len, name__ ## _path, replace_data__))

  _INSTALL(cmake_iowow_add, false);
  _INSTALL(cmake_iwnet_add, false);
  _INSTALL(cmake_ejdb2_add, false);
  _INSTALL(cmake_lists, true);
  _INSTALL(src_cmake_lists, true);
  _INSTALL(app_c, true);
  _INSTALL(app_h, true);
  _INSTALL(app_run, true);
  _INSTALL(test1, true);
  _INSTALL(tests_cmake_lists, true);
  _INSTALL(tools_cmake_lists, true);
  _INSTALL(tools_strliteral, false);
  _INSTALL(readme_md, true);
  _INSTALL(changelog, true);
  _INSTALL(uncrustify, false);
  _INSTALL(lvimrc, false);
  _INSTALL(gitignore, false);
  _INSTALL(license, true);
  RCC(rc, finish, _install_app_json(&ctx));


#undef _INSTALL

finish:
  iwhmap_destroy(ctx.keys);
  return rc;
}
