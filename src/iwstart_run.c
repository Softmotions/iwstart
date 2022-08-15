#include "iwstart.h"

#include "cmake_iowow_add.inc"
#include "cmake_iwnet_add.inc"
#include "cmake_ejdb2_add.inc"

#include "cmake_lists.inc"
#include "src_cmake_lists.inc"
#include "tests_cmake_lists.inc"

#include "app_h.inc"
#include "app_c.inc"
#include "app_run.inc"
#include "test1.inc"

#include "app_json.inc"
#include "readme_md.inc"

#include <iowow/iwlog.h>
#include <iowow/iwp.h>
#include <iowow/iwhmap.h>
#include <iowow/iwxstr.h>
#include <iowow/iwutils.h>

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
    return (void*) &g_env + off;
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
  "{project_base_lib_static}"
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

static iwrc _install(struct ctx *ctx, const unsigned char *data_, const unsigned int data_len, const char *path_) {
  iwrc rc = 0;
  char *path = 0, *data = 0;
  FILE *file = 0;
  IWXSTR *xstr = iwxstr_new();
  RCB(finish, xstr);
  RCC(rc, finish, _replace(ctx, path_, strlen(path_), &path));
  RCC(rc, finish, _replace(ctx, (char*) data_, data_len, &data));
  RCC(rc, finish, iwxstr_printf(xstr, "%s/%s", g_env.project_directory, path));
  RCC(rc, finish, iwp_mkdirs_for_file(iwxstr_ptr(xstr)));

  file = fopen(path, "w+");
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
  free(path);
  if (file) {
    fclose(file);
  }
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

#define _INSTALL(name__) \
  RCC(rc, finish, _install(&ctx, name__, name__ ## _len, name__ ## _path))

  _INSTALL(cmake_iowow_add);
  _INSTALL(cmake_iwnet_add);
  _INSTALL(cmake_ejdb2_add);
  _INSTALL(cmake_lists);
  _INSTALL(src_cmake_lists);
  _INSTALL(app_c);
  _INSTALL(app_h);
  _INSTALL(app_json);
  _INSTALL(readme_md);
  _INSTALL(test1);
  _INSTALL(tests_cmake_lists);
  _INSTALL(app_run);

#undef _INSTALL

finish:
  iwhmap_destroy(ctx.keys);
  return rc;
}
