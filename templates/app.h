#pragma once

#include <iowow/iwpool.h>
#include <iowow/iwrefs.h>
#include <pthread.h>

struct env {
  const char  *cwd;
  const char  *program;
  const char  *program_file;
  const char  *config_file;
  const char  *config_file_dir;
  const char **argv;
  struct iwpool      *pool;
  int  argc;
  bool verbose;
  volatile bool breakout_sigint;
};

extern struct env g_env;

void app_shutdown_request(void);

void app_shutdown_hook_register(void (*)(void));

iwrc app_thread_register(pthread_t);

void app_thread_unregister_join(pthread_t);

void app_thread_unregister_detach(pthread_t);

iwrc app_thread_run_ref(struct iwref_holder *ref, void (*)(void*), void* user_data);

iwrc app_thread_run(void (*)(void*), void* user_data);
