# Cubism SDK For Native Development

Welcome to the open components of the Cubism SDK For Native Development.
Came here looking for the official Live2D homepage? Go [here](http://www.live2d.com/products/cubism3).


## Design

The Cubism 3.0 SDK has been rewritten from scratch and much more 'low level' and modular than its predecessors.
It exposes a simple C APIs, and you can pick only the functionality you need.
(Rendering functions are completely optional, for example).
We have favoured this design approach because we think that instead of providing a complete but black-box library,
the native SDK should be as easy as possible to intergrate into your framework(s).

The component APIs are split into two headers: one 'public', and one 'internal' (the ones ending with `*INTERNAL.h`).
All types of non-fixed size are opaque in the public headers to provide a cleaner interface and
to abstract the memory layout of those types away.

Include the internal headers for memory layout of types and advanced usage.


## Components

### Framework

The framework contains functionality for playing back and blending animations, and for simulating physics.
It also provides convenience functions around the Cubism Core API.


### Reference Rendering Implementation

In most circumstances you're probably better off implementing rendering yourself, and
the Core API is designed to make this task as easy as possible.

The renderer included in this project is meant as a reference implementation,
so its main goal is to be easily readable and understandable and not performance.
(If the reference renderer suits your purposes, however, of course, feel free to use it).

If you don't want to start completely from scratch, the internal rendering header contains a 'barebone' renderer version,
that takes care of preparing and syncing vertices and indices with the GPU, while allowing you to render the way you want.


### Dependencies

Both framework and rendering depend on the C standard library and - obviously - the Core library.
They don't link against the Core itself, so make sure to allow them to link to the Core library as fit.

Furthermore, the rendering depends on OpenGLES 2.0 for mobile and on an OpenGL 3.3+ context for desktop.
While the rendering automatically includes the OpenGLES 2.0 header when compiling for mobile,
it doesn't include an OpenGL header when compiling for desktop.
This allows you to have the rendering use the OpenGL wrapper you want.

Check out the [sample](https://github.com/Live2D/CubismNativeComponents/blob/early-access/sample)
to get an idea how setting headers and link libraries can be done.


### Building

The most straightforward way to build the components is to use the [CMake project](https://github.com/Live2D/CubismNativeComponents/blob/early-access/CMakeLists.txt) provided.


## Sample

The project includes a sample that shows how you how to use the components.


#### Dependencies

The samples depend on the C standard library, the Cubism Core, [SDL2](https://www.libsdl.org),
[stb](https://github.com/nothings/stb)(, and [glad](https://github.com/Dav1dde/glad) on desktop).

Vendor dependencies are automatically downloaded as required using CMake and git.
The SDL team seems to favor building against dynamic SDL2 libraries, but we link against static ones in the sample.

Project files and makefiles can be generated with the [CMake project](https://github.com/Live2D/CubismNativeComponents/blob/early-access/sample/CMakeLists.txt) included.
Make sure to have CMake 3.6+ and git available prior generating the CMake project.


#### Build Prerequisites

1. Download the Cubism Core from [here](https://live2d.github.io/#native), extract it, and put the files in this repository into a subfolder as shown below.

```sh
.
+-- Live2DCubismSdk
|   +-- Core # <-- Included in zip
|   +-- Components # <-- Files from this repository 
```


#### Building Linux, macOS and Windows

1. Generate makefiles from the CMake project using [CMake](https://cmake.org/runningcmake/). ([Make sure to have a C++ toolset enabled on Windows](https://msdn.microsoft.com/en-us/library/x4d2c09s.aspx)).
1. Run `make`. (`nmake` on Windows). This will fetch all dependencies.
1. Generate the project once more. This will enable building the actual sample.
1. Run `make`. (`nmake` on Windows).
1. Run `./HelloCubism`.

#### Building Android

1. First build the sample for your host to make sure you have all dependencies required.
1. Import the SDL2 Android project included in SDL2 into Android Studio 2.2+.
1. Set up Gradle as shown below.
1. Edit the `SDL2Activity.java` to the native library as shown below.

`build.gradle(Module: app)`

```gradle
apply plugin: 'com.android.application'

android {
    compileSdkVersion 18
    buildToolsVersion "25.0.3"

    defaultConfig {
        applicationId "org.libsdl.app"
        minSdkVersion 18
        targetSdkVersion 18

        // Filter out ABIs not matching the distributed Core libraries.
        ndk {
            abiFilters "armeabi-v7a"
            abiFilters "x86"
            moduleName 'main'
        }

        // Enforce 'arm'.
        externalNativeBuild {
            cmake {
                arguments "-DANDROID_ARM_MODE=arm"
            }
        }
    }

    // Specify assets directory.
    sourceSets.main {
        assets.srcDirs = ["<path-to-sample-CMakeLists.txt>/assets"]
    }

    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.txt'
        }
    }

    // Set path to sample CMake project.
    externalNativeBuild {
        cmake {
            path "<path-to-sample-CMakeLists.txt>/CMakeLists.txt"
        }
    }
}
```

`SDLActivity.java`

```java
/**
  * This method is called by SDL before loading the native shared libraries.
  * It can be overridden to provide names of shared libraries to be loaded.
  * The default implementation returns the defaults. It never returns null.
  * An array returned by a new implementation must at least contain "SDL2".
  * Also keep in mind that the order the libraries are loaded may matter.
  * @return names of shared libraries to be loaded (e.g. "SDL2", "main").
  */
protected String[] getLibraries() {
    return new String[] {
        "HelloCubism"
    };
}
```

### Building iOS

1. First build the sample for your host to make sure you have all dependencies required.
1. Open the SDL2 `Xcode-iOS/SDL/SDL.xcodeproj` and build a static library for your target device.
1. Create a SDL2 template project. (If you're doing this for the first time, [here's a nice walkthrough](http://lazyfoo.net/tutorials/SDL/52_hello_mobile/ios_mac/index.php)).
1. Add `AVFoundation.framework` to `Frameworks`. (Otherwise, [you get some nasty link errors](https://discourse.libsdl.org/t/improving-the-ios-experience/22623/7)).
1. Add the Core iOS library.
1. Add the source files from `./sample/src` and `./src` to the project.
1. Add the `./include`, `./sample/vendor/stb-master` and the directory containing the Core header file to include paths.
1. Add `_USE_GLES20`, `_IOS`, `_CSM_COMPONENTS_IOS`, `_CSM_COMPONENTS_USE_GLES20` to preprocessor defines.
1. Add the files from `./sample/assets` as resources to the project.
1. Build the project.


## Code Snippets

#### Instantiating a Model.

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

For more snippets-like code, check out `./sample/src/HelloCubism.c`. 


## Contributing

There are many ways to contribute to the project: logging bugs, submitting pull requests, reporting issues, and creating suggestions.

### Coding Convention

- **Avoid static and global variables** (unless they really help in providing a much cleaner API).
- Use upper camelcase for types, functions and fields, e.g. `ReadMotionJson()`.
- Use lower camelcase for local variables, e.g. `modelSize`.
- Choose [meaningful names](https://docs.unrealengine.com/latest/INT/Programming/Development/CodingStandard/#examples) for types, functions, fields, and variables.
- Start function names with a verb and make them as readable as English sentences as possible, e.g. `EvaluateAnimationSegment()` (and not ~~`AnimationSegmentEvaluate()`~~).
- Use the internal headers (`*INTERNAL.H`) for providing additional functionality to advanced while keeping the public header clean.
- Keep public and internal headers clean by anything that doesn't serve the API in source files and local header files.
- Prefix all declarations in public and internal headers with `csm` (followed by upper camelcase), i.e. `csmModel`.


## Discussion Etiquette

Please limit the discussion to English and keep it professional and things on topic.


## TODO

- Add more code snippets.

## License

The license applying to the source code in this project allows you modify all sources without the need to submit any changes you made.
Whenever releasing a product using source code from this project,
you just have to make sure that you link your product with the Core distributed with the SDK package.
Refer to [this license](http://live2d.com/eula/live2d-open-software-license-agreement_en.html) for the gritty details.
