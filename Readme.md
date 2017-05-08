# Cubism SDK For Native Development

Welcome to the open components of the Cubism SDK For Native Development.
While the Cubism Core, which you can obtain from [here](https://live2d.github.io),
is feature-complete in regard to Cubism 3.0 features and well tested (it actually powers the SDK For Unity),
**the open components are work-in-progress**.
They're not yet feature-complete and their API isn't fixed yet (and the samples are only tested on Linux, macOS and Windows).

If you're interested in why we're releasing the SDK anyway as early access, read on.

If you came here looking for official Live2D homepage, instead, you'll find it [here](http://www.live2d.com/products/cubism3).


## Why Early Access?

With the Cubism 3.0 SDKs we strive to provide the SDKs you need.
We shared the Unity SDK from the prototype stage with multiple developers and made sure to reflect their feedback.

For the native SDK we take things one step further by opening the evaluation phase up for everyone,
so it would be great if you could give the SDK a try and provide any feedback through this GitHub project.


## Design

The Cubism 3.0 SDK has been rewritten from scratch and much more 'low level' and modular than its predecessors.
It exposes a simple C APIs, and you can pick only the functionality you need.
(Rendering functions are completely optional, for example).
We have favoured this design approach because we think that instead of providing a complete, but black-box library
the native SDK should be as easy as possible to intergrate into your framework(s).

The component APIs are split into two headers: one 'public', and one 'internal' (the one ending with `*Internal.h`).
All types of non-fixed size are opaque in the public headers to provide a cleaner header and
abstraction layer from the raw memory layout of those types.

Include the internal headers for the memory layout of types and advanced usage.


### Framework

The framework contains functionality for playing back and blending animations as well as convenience functions for the Cubism Core.

#### Dependencies

The framework only depends on the C standard library (and the Cubism Core library, obviously).


### Reference Rendering Implementation

In most circumstances you're probably better off implementing rendering yourself, and
the Core API is designed to make this task as easy as possible.

The renderer included in this project is meant as a reference implementation,
so its main goal is to be easily readable and understandable and not performance.
(If the reference renderer suits your purposes, however, of course, feel free to use it).

If you don't want to start completely from scratch, the internal rendering header contains a 'barebone' renderer version,
that takes care of preparing and syncing vertices and indices with the GPU, while allowing you to render the way you want.

#### Dependencies

The implementation depends on the C standard library, the Cubism Core library; OpenGL rendering functionality on OpenGL.
On desktop OpenGL functions are loaded using [glad](https://github.com/Dav1dde/glad);
on mobile OpenGLES 2.0 is used (including the extensions
[OES_mapbuffer](https://www.khronos.org/registry/OpenGL/extensions/OES/OES_mapbuffer.txt) and
[OES_vertex_array_object](https://www.khronos.org/registry/OpenGL/extensions/OES/OES_vertex_array_object.txt)).


## Samples

The project includes 2 samples to get you started:

1. `./Samples/HelloCubism.c` shows the basics of manually controlling a model.
1. `./Samples/HelloAnimation.c` shows how to animate a model by blending animations.


#### Dependencies

The samples depend on the C standard library, the Cubism Core, [SDL2](https://www.libsdl.org),
[stb](https://github.com/nothings/stb), and [glad](https://github.com/Dav1dde/glad).

Except SDL2, vendor dependencies are automatically downloaded on first build.

Project files and makefiles can be generated with the [CMake project](https://github.com/Live2D/CubismNativeComponents/blob/early-access/CMakeLists.txt) included.
Make sure to have CMake 3.6+ and git available prior generating using the CMake project.


#### Building

1. Download the Cubism Core from [here](live2d.github.io) and put the deflated files into the root directory of this project.
1. Download [SDL 2.0.5 source code](https://www.libsdl.org/download-2.0.php) and put deflated folder into `./Samples/vendor`.
1. Generate make or project files for Linux, macOS or Windows using [CMake](https://cmake.org/runningcmake/).
1. Build the samples and run them from the build directory.


## Code Snippets

#### Instantiating A Model.

*Be aware that memory alignment constraints exist when instantiating models*.

```c
#include <Live2DCubismCore.h>


// 1. Get size of moc file,
// 2. allocate buffer for moc file of adequate size and aligned to 'csmAlignofMoc', and
// 3. read moc file into buffer.


// 4. Restore moc in place. The 'mocBuffer' pointer will be returned on success.
csmMoc* moc = csmReviveMocInPlace(mocBuffer, mocSize);


// 5. Get size of model in bytes.
const unsigned int modelSize = csmGetSizeofModel(moc);


// 6. Allocate buffer for model with adequate size and aligned to 'csmAlignofModel', then...


// 7. Instantiate model in place. The 'modelBuffer' pointer will be returned on success.
csmModel* model = csmInitializeModelInPlace(moc, modelBuffer, modelSize);


// As you are the owner of both moc and model memory, you can free both whenever you want.
```

For more snippets-like code, check out `./Samples/src/SampleModel.c`. 


## Contributing

There are many ways to contribute to the project: logging bugs, submitting pull requests, reporting issues, and creating suggestions.

While any form of contributing is greatly appreciated, *suggestions regarding design and API are especially important to us* on this GitHub repository.


## Discussion Etiquette

Please limit the discussion to English and keep it professional and things on topic.


## Todo

- Test and debug OpenGLES 2.0 renderer.
- Add more code snippets.
- Implement physics.
- Implement API usage sanity checks.
- Add document on coding style.
- Profile and optimize framework.
- Provide instructions for building samples on mobile. 

## License

The license that applies to the source code in this project allows you modify all sources without the need to submit any changes you made.
Whenever releasing a product using source code from this project,
you just have to make sure that you link your product with the Core distributed with the SDK package.
Refer to [this license](http://live2d.com/eula/live2d-open-software-license-agreement_en.html) for the gritty details.
