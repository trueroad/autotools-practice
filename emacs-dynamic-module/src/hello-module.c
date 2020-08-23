#ifdef _WIN32
#define HELLO_MODULE_DLL __declspec(dllexport)
#else
#define HELLO_MODULE_DLL
#endif

#include "config.h"

#include <emacs-module.h>
#include <string.h>

int HELLO_MODULE_DLL plugin_is_GPL_compatible;

static  emacs_value
hello_message (emacs_env *env, ptrdiff_t nargs, emacs_value *args, void *data)
{
  emacs_value message = env->intern (env, "message");
  char hello_str[] = "Hello World";
  emacs_value hello = env->make_string (env, hello_str, strlen (hello_str));
  emacs_value hello_args[] = {hello};
  env->funcall (env, message, sizeof (hello_args) / sizeof (hello_args[0]),
		hello_args);

  return hello;
}

int HELLO_MODULE_DLL
emacs_module_init (struct emacs_runtime *ert)
{
  if (ert->size < sizeof (*ert))
    return 1;

  emacs_env *env = ert->get_environment (ert);
  if (env->size < sizeof (*env))
    return 2;

  {
    emacs_value defalias = env->intern (env, "defalias");
    emacs_value symbol = env->intern (env, "hello-message");
    emacs_value func =
      env->make_function (env, 0, 0, hello_message,
			  "Hello World by dynamic module", NULL);
    emacs_value args[] = {symbol, func};
    env->funcall (env, defalias, sizeof (args) / sizeof (args[0]), args);
  }

  {
    emacs_value provide = env->intern (env, "provide");
    emacs_value feature = env->intern (env, "hello-module");
    emacs_value args[] = {feature};
    env->funcall (env, provide, sizeof (args) / sizeof (args[0]), args);
  }

  return 0;
}
