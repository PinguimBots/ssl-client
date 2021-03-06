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
and also python3, then you run
```
$ python3 -m pip install meson
```

> *On most distros you __CAN__ also do*
> ```
> $ sudo apt install meson
> ```
> *But you really should install it using pip if you want to avoid problems.*

Qt is a whole can of worms i won't get into but just go to the [official
site](https://qt.io) and follow the instructions.

Then, setup the build system with
```
$ meson builddir [-Dfixed-team-size-of=SIZE]
```

Where optional arguments are wrapped in brackets.
> *With MSVC you might need to do*
> ```
> $ meson builddir -Ddefault_library=static [-Dfixed-team-size-of=SIZE]
> ```
> *To avoid link errors later on.*

And lastly, compile and run the project with
```
$ meson compile -C builddir && ./builddir/pbssl
```

And you're done! :tada: :tada: :tada:
