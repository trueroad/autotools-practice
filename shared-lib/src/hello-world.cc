#ifdef _WIN32
#define HELLO_WORLD_DLL __declspec(dllexport)
#else
#define HELLO_WORLD_DLL
#endif

#include "hello-world.hh"
#include "config.h"

#include <iostream>

int HELLO_WORLD_DLL hello_world (void)
{
  std::cout << "Hello World" << std::endl;

  return 0;
}
