# RTRender

## Build
  1. Create build directory somewhere `mkdir /path/to/build`
  2. Go to the build directory `cd /path/to/build`
  3. Run cmake `cmake /path/to/RTRender`
  4. Run make `make`

## Run
  * `-s <sample>`    draws a hard-coded geometric sample named `<sample>`
    #### List of possible `<sample>` variants:
        1. `spiral`
        2. `golden_spiral`
        3. `triangles`
      
          
  * `-o <object>`    opens `.obj` file named `<object>`
  * `-m <mode> `     chooses a way to render the `<object>`

#### List of possible `<mode>` variants:
        1. `wire`           - draws wireframe of the model
        2. `rasterize`      - rasterizes the model (uses z-buffer)
        3. `texture`      - rasterizes model with texture `.tga` file (if the model name is `model.obj` textures must be named `model_diffuse.tga`)
        4. `zbuf`       - renders z-buffer of the model
        5. `rand`           - paints model in random colors
        6. `dont_remove`    - rasterizes the model (no z-buffer)
      
  * `-h`     shows usage info
