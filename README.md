# ftracer
![Example scene (1024 spp, 4 subpixels)](https://raw.githubusercontent.com/phonxvzf/fur/master/example.jpg)

A physically-based image synthesizer based on ray tracing methods.

_f_ in _ftracer_ stands for _fur_, not grade F.

_This project is currently in progress._

## Features
- Path-traced global illumination (unbiased Monte Carlo)
- Microfacets BSDF
- Stratified uniform samples
- Russian roulette path termination
- Bounding volume hierarchy (BVH) with surface area heuristic (SAH)
- Hair geometry intersection accelerated using Intel® Embree
- Anti-aliasing achieved via subpixel supersampling with Blackman-Harris filter
- Multi-threaded rendering (C++ STL threads)
- Object loading via Assimp
- Loading up Cem Yuksel's hair file
- Path-traced subsurface-scattering via volumetric approach
- Path-traced hair/fur BSDF
- Multiple importance sampling for single light source (can be turned off)
- YAML scene file

## Important note about coordinate system
Although other renderers like to use right-handed coordinate system, ftracer uses left-handed coordinate system where `+Y` axis shoots up vertically, `+X` points to the right and `+Z = +X cross +Y`. This is due to the fact that I'm used to OpenGL's coordinate system.

## Dependencies
- CMake >= 3.7
- A sane GNU/Linux environment
- GNU C++ compiler with C++17 support (Since it uses `__builtin_clz` and Bessel function `I0`.)
- OpenEXR >= 2.4.0
- yaml-cpp >= 0.6.3
- Assimp >= 5.0.0
- Intel® Embree >= 3.8.0
- tcmalloc from gperftools

## Building
```
$ cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug .
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
If you have problems displaying the progress bar, please consider using `--quiet` option.

### Command line options
- `-h or --help` print help message and exit gracefully
- `-j n` use `n` number of threads to render
- `-d or --depth` output the rays' depth that hit a surface instead of color spectrum. 0 is defined as not hit.
- `-n or --normal` output intersected faces normal vectors (in world space)
- `-s or --start` and `-e or --end` only render a rectangular portion of the image, very useful while debugging
- `-o output` write output in floating point EXR format to file name `output`. The program defaults `output` to `output.exr` if this option is not set. Note that some pixel values can be larger than 1 or even negative.
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
- Dipole BSSRDF
