- add examples
- add simplest example to README.md
- rework to remove chaining limitation
- hide the self parameter for yield on the stack
- add selling points
    - doesnt us setjmp/longjmp
    - tiny source footprint
    - simple api
    - cross platform
    - cross toolchain
    - just coroutines
    - ideal for games and AI
- port to C
- benchmark
    - yields per second
- compare to other coroutine libs
    - fibers
- port to arm
- use -m32 to test out x86 linux
- make sure float registers are not clobbered

. This is required on linux for x86 builds:
  sudo apt-get install g++-multilib libc6-dev-i386
  CMAKE_CXX_FLAGS = -m32
  CMAKE_C_FLAGS = -m32
