
# pbssl
> PinguimBots client for the 2020 online #robocub competition 

## Hard Dependencies
Needed to run the project:
- [Qt](https://www.qt.io/) >= 5.10
- [meson](https://github.com/mesonbuild/meson) >= 0.55

## Soft Dependencies
Downloaded automatically if needed:
- [{fmt}](https://fmt.dev) >= 7.0.0
- [docopt.cpp](https://github.com/docopt/docopt.cpp) >= 0.6.0
- [glm](http://glm.g-truc.net/) >= 0.9.8
- [protobuf](https://developers.google.com/protocol-buffers) >= 3.0.0

## Running the project

Firstly you are going to need to install the dependencies.

To install meson you need a backend such as ninja or whatever MSVC uses
and also python3, then you __SHOULD__ (on most distributions) do
```
$ python3 -m pip install meson
```
But you __CAN__ do
```
$ sudo apt install meson
```

Qt is a whole can of worms i won't get into but just go to the [official
site](https://qt.io) and follow the instructions.

Then, setup the build system with
```
$ meson builddir
```

> *With MSVC you might need to do*
> ```
> $ meson builddir -Ddefault_library=static
> ```
> *To avoid link errors later on.*

Then, compile and run the project with
```
$ meson compile -C builddir && ./builddir/pbssl
```

And you're done! :tada: :tada: :tada:
