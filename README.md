# ftracer
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
$ ./ftracer [ -h | -j workers | -o output | --help | --quiet ] SCENE
```
The program automatically detects the number of your processor concurrency (typically your number of logical cores).
You can override this by supplying `-j N` option, where `N` is the number of render workers to spawn.

Please also note that the program assumes that your terminal supports unicode characters.
If you have problems displaying the progress bar, please consider use `--quiet` option ;-P.

Example:
```
$ ./ftracer -j 8 -o output.exr scene.yaml
```

## YAML Scene File
Please see `example_scene.yaml` for references.
