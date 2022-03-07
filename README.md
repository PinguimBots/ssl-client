# pbssl
> PinguimBots client for the 202X online/offline #robocub competitions

## Dependencies
In order to run the project you need **installed on your system**:
- [meson](https://github.com/mesonbuild/meson) >= 0.60 and a suitable backend (ninja/make)
- A C++20 capable compiler [(check here)](https://en.cppreference.com/w/cpp/compiler_support)
    - GCC >= 11 OR
    - Clang >= 13
- OpenGL
- OpenMP

The following are also needed but downloaded/compiled automatically if not found:
- [SDL2](https://www.libsdl.org/) >= 2.0.9
- [glew](http://glew.sourceforge.net/) >= 2.0.0
- [imgui](https://github.com/ocornut/imgui) >= 1.7.6
- [asio](https://think-async.com/Asio/) >= 1.18.1
- [{fmt}](https://fmt.dev) >= 7.0.0
- [docopt.cpp](https://github.com/docopt/docopt.cpp) >= 0.6.0
- [glm](http://glm.g-truc.net/) >= 0.9.8
- [protobuf](https://developers.google.com/protocol-buffers) >= 3.0.0 and protoc

The following are used when present but not necessary:
- [OpenCV4](https://opencv.org/) >= 4.5.1

## Running the project

Firstly you are going to need to install meson, to do that you should follow [the Quickstart Guide](https://mesonbuild.com/Quick-guide.html).

Then just running the project with `./run.sh <program arguments>` will do everything else for you.

However, if you need a little more control over the build configuration you can do so by running `./configure.sh <meson arguments>` and then doing the same as above.

And you're done! :tada: :tada: :tada:
