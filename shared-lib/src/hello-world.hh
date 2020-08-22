#ifndef INCLUDE_GUARD_HELLO_WORLD_HH
#define INCLUDE_GUARD_HELLO_WORLD_HH

#ifdef _WIN32
#ifndef HELLO_WORLD_DLL
#define HELLO_WORLD_DLL __declspec(dllimport)
#endif
#else
#ifndef HELLO_WORLD_DLL
#define HELLO_WORLD_DLL
#endif
#endif

int HELLO_WORLD_DLL hello_world (void);

#endif // INCLUDE_GUARD_HELLO_WORLD_HH
