# ftracer
![Example scene (1024 spp, 4 subpixels)](https://raw.githubusercontent.com/phonxvzf/fur/master/example.jpg)

A physically-based image synthesizer using ray marching methods.

_This project is currently in progress._

## Prerequisites
- CMake >= 3.7
- A sane GNU/Linux environment
- C++ compiler with C++14 support (most compilers support this)
- OpenEXR library
- yaml-cpp library

## Building
```
$ cmake -B build -DCMAKE_BUILD_TYPE=Debug .
$ cd build
$ make -j8
```
For release build, replace `Debug` with `Release`

## Usage
```
$ cd build
$ ./ftracer [ -h | -j workers | -d | -o output | --help | --quiet | --depth ] SCENE
```
The program automatically detects the number of your processor concurrency (typically your number of logical cores).
You can override this by supplying `-j N` option, where `N` is the number of render workers to spawn.

Please also note that the program assumes that your terminal supports Unicode characters.
If you have problems displaying the progress bar, please consider use `--quiet` option :-P.

### Command line options
- `-h or --help` print help message and exit gracefully
- `-j n` use `n` number of threads to render
- `-d or --depth` output the rays' depth that hit a surface instead of color spectrum. -1 is defined as not hit.
- `-o output` write output in EXR format to file name `output`. The program defaults `output` to `output.exr` if this option is not set.
- `--quiet` do not report anything (This option sets `std::cout` and `std::wcout` state to `std::ios::failbit`.)

Examples:
```
$ ./ftracer -d -o scene.exr scene.yaml
$ display scene.exr

$ ./ftracer -j 8 -o scene.exr scene.yaml
$ display scene.exr

$ ./ftracer scene.yaml --quiet
$ display output.exr
```

## YAML Scene File
Please see `example_scene.yaml` for references.
