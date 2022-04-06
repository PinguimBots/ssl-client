# pbssl
> PinguimBots client for the 202X online/offline #robocub competitions

## Dependencies

Sometimes we need to run this repo on college machines (typically CentOS7) which doesn't have
every tool we need, so the following tooling is necessary but downloaded/compiled locally when not found.

- [Python](https://www.python.org/) `>= 3.9`
- [meson](https://github.com/mesonbuild/meson) `>= 0.60`
- A meson backend like [ninja](https://ninja-build.org/) or VS20YY
- A C++20 capable compiler [(check here)](https://en.cppreference.com/w/cpp/compiler_support)
    - GCC `>= 11` OR
    - Clang `>= 13` OR
    - MSVC `>= 19` and VS `>= 16.10`

> *Note: Bootstrapped tooling needs `make`, `wget`, `git`, `tar` and `grep` installed.*

Dependency-wise you will need **installed on your system**:

- OpenGL
- OpenMP

Also, the following are necessary, but are downloaded/compiled automatically if not found:

- [SDL2](https://www.libsdl.org/) `>= 2.0.9`
- [glew](http://glew.sourceforge.net/) `>= 2.0.0`
- [imgui](https://github.com/ocornut/imgui) `>= 1.7.6`
- [asio](https://think-async.com/Asio/) `>= 1.18.1`
- [{fmt}](https://fmt.dev) `>= 7.0.0`
- [glm](http://glm.g-truc.net/) `>= 0.9.8`
- [protobuf](https://developers.google.com/protocol-buffers) `>= 3.0.0` and protoc

And the following dependencies are used when present but not strictly necessary:

- [OpenCV4](https://opencv.org/) `>= 4.5.1`

## Running the project

Just running `./run.sh -- <program arguments>` will do compile/download everything
and run the executable.

However, if you need a little more control over the build configuration
you can do so by running `./configure.sh -- <meson arguments>` and then
doing the same as above.

And you're done! :tada: :tada: :tada:
