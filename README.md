# RTRender

## Build
  1. Create build directory somewhere `mkdir /path/to/build`
  2. Go to the build directory `cd /path/to/build`
  3. Run cmake `cmake /path/to/RTRender`
  4. Run make `make`

## Run
  * `-g` draws polyline golden spiral
  * `-ng` drawn polyline non-golden spiral
  * `-t` draws triangles
  * `-wire /path/to/obj/file` draws wireframe model
  * `-rand /path/to/obj/file` paints wireframe model in random colors
  * `-rast /path/to/obj/file` rasterizes model
  * `-remov /path/to/obj/file` rasterizes model removing back sides

