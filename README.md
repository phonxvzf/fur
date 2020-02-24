# ftracer
![Example scene (1024 spp, 4 subpixels)](https://raw.githubusercontent.com/phonxvzf/fur/master/example.jpg)

A physically-based image synthesizer using ray tracing methods.

_This project is currently in progress._

## Features
- Path-traced global illumination (unbiased Monte Carlo)
- GGX BSDF for both reflection and refraction
- BSDF importance sampling
- Stratified uniform samples
- Russian roulette path termination
- Bounding volume hierarchy (BVH) with surface area heuristic (SAH)
- Anti-aliasing achieved via subpixels with average filter
- Multi-threaded rendering (C++ STL threads)
- Object loading via Assimp
- Path-traced subsurface-scattering (experimental)
- Path-traced hair/fur BSDF (experimental)
- YAML scene file

## Prerequisites
- CMake >= 3.7
- A sane GNU/Linux environment
- GNU C++ compiler with C++17 support (Since it uses `__builtin_clz`.)
- OpenEXR >= 2.4.0
- yaml-cpp >= 0.6.3
- Assimp >= 5.0.0

## Building
```
$ cmake -B build -DCMAKE_BUILD_TYPE=Debug .
$ cd build
$ make
```
For release build, replace `Debug` with `Release`

## Usage
```
$ cd build
$ ./ftracer [OPTIONS] SCENE
```
The program automatically detects the number of your processor concurrency (typically your number of logical cores).
You can override this by supplying `-j N` option, where `N` is the number of render workers to spawn.

Please also note that the program assumes that your terminal supports Unicode characters.
If you have problems displaying the progress bar, please consider use `--quiet` option :-P.

### Command line options
- `-h or --help` print help message and exit gracefully
- `-j n` use `n` number of threads to render
- `-d or --depth` output the rays' depth that hit a surface instead of color spectrum. 0 is defined as not hit.
- `-n or --normal` output intersected faces normal vectors (in world space)
- `-s or --start` and `-e or --end` only render a rectangular portion of the image, very useful while debugging
- `-o output` write output in EXR format to file name `output`. The program defaults `output` to `output.exr` if this option is not set.
- `--quiet` do not report anything (This option sets `std::cout` and `std::wcout` state to `std::ios::failbit`.)

Examples:
```shell
$ ./ftracer -d -o scene_depth.exr scene.yaml
$ display scene_depth.exr

$ ./ftracer -n -o scene_normal.exr scene.yaml
$ display scene_normal.exr

$ ./ftracer scene.yaml -s 320 0 -e 640 480
$ display output.exr # Left half of the image will be black.

$ ./ftracer -j 8 -o scene.exr scene.yaml
$ display scene.exr

$ ./ftracer scene.yaml --quiet
$ display output.exr
```

## YAML Scene File
Please see `example_scene.yaml` for references.

## TODOs
- Dual scattering for hair
- BSSRDF
- FXAA
