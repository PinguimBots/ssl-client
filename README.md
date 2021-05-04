# pbssl
> PinguimBots client for the 202X online #robocub competitions

## Dependencies
In order to run the project you need **installed on your system**:
- [OpenCV](https://opencv.org/) >= 4.0.0
- [Qt](https://www.qt.io/) >= 5.9
- [meson](https://github.com/mesonbuild/meson) >= 0.56 and a suitable backend
- OpenGL
- OpenMP

The following are also needed but downloaded automatically if not found:
- [SDL2](https://www.libsdl.org/) >= 2.0.9
- [glew](http://glew.sourceforge.net/) >= 2.0.0
- [imgui](https://github.com/ocornut/imgui) >= 1.7.6
- [{fmt}](https://fmt.dev) >= 7.0.0
- [docopt.cpp](https://github.com/docopt/docopt.cpp) >= 0.6.0
- [glm](http://glm.g-truc.net/) >= 0.9.8
- [protobuf](https://developers.google.com/protocol-buffers) >= 3.0.0

## Running the project

Firstly you are going to need to install the **hard dependencies**:
- To install meson you should follow [the Quickstart Guide](https://mesonbuild.com/Quick-guide.html).
- For Qt you should head to the [official site](https://qt.io) and follow the instructions.
- Finally, for OpenCV go to the [official site](https://opencv.org/) and follow the instructions.

Then just running the project with `./run.sh <program arguments>` will do everything else for you.

However, if you need a little more control over the build configuration you can
do so by running `./configure.sh <meson arguments>` and then doing the same as above.

And you're done! :tada: :tada: :tada:
