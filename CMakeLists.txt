# ---- #
# META #
# ---- #

cmake_minimum_required(VERSION 3.6)
project(Live2DCubismComponents C)


# -------- #
# REQUIRES #
# -------- #

include(ExternalProject)


# ------------ #
# USER OPTIONS #
# ------------ #

# Path to native Cubism Core.
set(CSM_CORE_DIRECTORY "./Live2DCubismCore" CACHE STRING "Path to Live2D Cubism Core for native development.")

# Control for building OpenGL renderer.
option(CSM_BUILD_OPENGL "Enables building of OpenGL reference rendering implenentation." OFF)

# Control for building samples.
option(CSM_BUILD_SAMPLES "Enables building of samples." ON)


# ------------ #
# PUBLIC LISTS #
# ------------ #

# Contains Cubism Core and components include directories.
set(CSM_INCLUDE_DIRS "")


# Contains Cubism core and components libraries as well as their dependencies.
set(CSM_LIBS "")


# ---------------- #
# INTERNAL OPTIONS #
# ---------------- #

# Find Cubism Core root and include directory.
if (NOT IS_ABSOLUTE CSM_CORE_DIRECTORY)
  set(_CSM_CORE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${CSM_CORE_DIRECTORY})
else ()
  set(_CSM_CORE_ROOT_DIR ${CSM_CORE_DIRECTORY})
endif ()


set(_CSM_CORE_INCLUDE_DIR ${_CSM_CORE_ROOT_DIR}/include)


# Set component and sample directories shorthands.
set(_CSM_COMPONENTS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Live2DCubismComponents)

set(_CSM_COMPONENTS_INCLUDE_DIR ${_CSM_COMPONENTS_ROOT_DIR}/include)


set(_CSM_FRAMEWORK_SOURCE_DIR ${_CSM_COMPONENTS_ROOT_DIR}/src/Framework)
set(_CSM_RENDERING_SOURCE_DIR ${_CSM_COMPONENTS_ROOT_DIR}/src/Rendering)


# Set convenience build type identifier.
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(_CSM_BUILD_RELEASE ON)
else ()
  set(_CSM_BUILD_DEVELOP ON)
endif ()


# Always enable framework build.
set(_CSM_BUILD_FRAMEWORK ON)


# Enable OpenGL rendering build if requested (or implicitly necessary).
if (CSM_BUILD_OPENGL OR CSM_BUILD_SAMPLES)
  set(_CSM_BUILD_OPENGL ON)
endif ()


# Set convenience flag for checking whether rendering build is requested.
if (_CSM_BUILD_OPENGL)
  set(_CSM_BUILD_RENDERING ON)
endif ()


# Set OpenGL rendering options.
if (_CSM_BUILD_OPENGL)

  # Use OpenGLES 2.0 on mobile...
  if (ANDROID OR IOS)
    set(_CSM_USE_GLES20 ON)


  # ... and OpenGL 3.3 with glad on desktop.
  else ()
    set(_CSM_USE_GL33 ON)
    set(_CSM_INCLUDE_GLAD ON)


    # Use glad debug in debug builds.
    if (_CSM_BUILD_DEVELOP)
      set(_CSM_GLAD_ROOT_DIR ${_CSM_COMPONENTS_ROOT_DIR}/vendor/glad-debug)
    else ()
      set(_CSM_GLAD_ROOT_DIR ${_CSM_COMPONENTS_ROOT_DIR}/vendor/glad)
    endif ()
  endif ()
endif ()


# Enable samples if requested.
if (CSM_BUILD_SAMPLES)
  set(_CSM_BUILD_SAMPLES ON)


  # TODO  Add support for more platforms.
  if (NOT APPLE AND NOT WIN32)
    message(FATAL_ERROR "Samples can currently only by built for macOS and Windows.")
  endif ()
endif ()


# ------------- #
# SANITY CHECKS #
# ------------- #

# TODO  Add.


# --------- #
# FRAMEWORK #
# --------- #

if (_CSM_BUILD_FRAMEWORK)

  # Configure library.
  add_library(Live2DCubismFramework STATIC
    ${_CSM_FRAMEWORK_SOURCE_DIR}/Animation.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/AnimationSegmentEvaluationFunction.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/AnimationState.c

    ${_CSM_FRAMEWORK_SOURCE_DIR}/FloatBlendFunction.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/FloatSink.c

    ${_CSM_FRAMEWORK_SOURCE_DIR}/ModelExtensions.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/ParameterExtensions.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/PartExtensions.c

    ${_CSM_FRAMEWORK_SOURCE_DIR}/Hash.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/Json.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/MotionJson.c
    ${_CSM_FRAMEWORK_SOURCE_DIR}/String.c)


  target_include_directories(Live2DCubismFramework PRIVATE
    ${_CSM_CORE_INCLUDE_DIR}
    ${_CSM_COMPONENTS_INCLUDE_DIR})


  # Add info.
  list(APPEND CSM_INCLUDE_DIRS ${_CSM_COMPONENTS_INCLUDE_DIR})
  list(APPEND CSM_LIBS Live2DCubismFramework)
endif ()


# --------- #
# RENDERING #
# --------- #

if (_CSM_BUILD_RENDERING)

  # Initialize 'local' lists.
  set(_CSM_RENDERING_INCLUDE_DIRS 
    ${_CSM_CORE_INCLUDE_DIR}
    ${_CSM_COMPONENTS_INCLUDE_DIR})


  set(_CSM_RENDERING_SOURCE_FILES
    ${_CSM_RENDERING_SOURCE_DIR}/RenderDrawable.c
    ${_CSM_RENDERING_SOURCE_DIR}/SortableDrawable.c)


  set(_CSM_RENDERING_DEFINES "")


  # Include OpenGL rendering in build if requested...
  if (_CSM_BUILD_OPENGL)
    list(APPEND _CSM_RENDERING_SOURCE_FILES
      ${_CSM_RENDERING_SOURCE_DIR}/GlBuffer.c
      ${_CSM_RENDERING_SOURCE_DIR}/GlDraw.c
      ${_CSM_RENDERING_SOURCE_DIR}/GlLoad.c
      ${_CSM_RENDERING_SOURCE_DIR}/GlMaskbuffer.c
      ${_CSM_RENDERING_SOURCE_DIR}/GlProgram.c
      ${_CSM_RENDERING_SOURCE_DIR}/GlRenderer.c
      ${_CSM_RENDERING_SOURCE_DIR}/GlVersion.c)


    # Make sure to include glad on desktop...
    if (_CSM_INCLUDE_GLAD)
      list(APPEND _CSM_RENDERING_INCLUDE_DIRS ${_CSM_GLAD_ROOT_DIR}/include)
      list(APPEND _CSM_RENDERING_SOURCE_FILES ${_CSM_GLAD_ROOT_DIR}/src/glad.c)
    endif ()


    # ... and to allow linking to OpenGL on non-Windows platforms.
    if (NOT WIN32)
      find_package(OpenGL REQUIRED)


      list(APPEND CSM_LIBS ${OPENGL_LIBRARIES})
    endif()


    # Set defines.
    if (_CSM_USE_GLES20)
      list(APPEND _CSM_RENDERING_DEFINES -D_CSM_USE_GLES20 -D_CSM_INCLUDE_GLES20)
    endif ()

    if (_CSM_USE_GL33)
      list(APPEND _CSM_RENDERING_DEFINES -D_CSM_USE_GL33)
    endif()

    if (_CSM_INCLUDE_GLAD)
      list(APPEND _CSM_RENDERING_DEFINES -D_CSM_INCLUDE_GLAD)
    endif ()


  # ... and a stub otherwise.
  else ()
    list(APPEND _CSM_RENDERING_SOURCE_FILES ${_CSM_RENDERING_SOURCE_DIR}/GlStub.c)
  endif ()


  # Configure library.
  add_library(Live2DCubismRendering STATIC ${_CSM_RENDERING_SOURCE_FILES})


  target_include_directories(Live2DCubismRendering PRIVATE ${_CSM_RENDERING_INCLUDE_DIRS})
  target_compile_definitions(Live2DCubismRendering PRIVATE ${_CSM_RENDERING_DEFINES})


  # Add info.
  list(APPEND CSM_INCLUDE_DIRS ${_CSM_COMPONENTS_INCLUDE_DIR})
  list(APPEND CSM_LIBS Live2DCubismRendering)
endif ()


# ------- #
# SAMPLES #
# ------- #

if (_CSM_BUILD_SAMPLES)

  # Initialize 'local' lists.
  set(_CSM_SAMPLES_INCLUDE_DIRS ${CSM_INCLUDE_DIRS})
  set(_CSM_SAMPLES_LIBS ${CSM_LIBS})
  set(_CSM_SAMPLES_SOURCE_FILES "")
  set(_CSM_SAMPLES_DEFINES "")


  # Build SDL2.
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON  CACHE BOOL "" FORCE)

  
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Samples/vendor/SDL2-2.0.5)


  # Download stb.
  ExternalProject_Add(Stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG "master"
    

    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/Samples/vendor/stb"


    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND "")


  # Include glad on desktop.
  if (_CSM_INCLUDE_GLAD)
    add_library(Glad STATIC ${_CSM_GLAD_ROOT_DIR}/src/glad.c)

    target_include_directories(Glad PRIVATE ${_CSM_GLAD_ROOT_DIR}/include)
  endif ()


  # Assemble include directories and libraries.
  list(APPEND _CSM_SAMPLES_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Samples/include)


  list(APPEND _CSM_SAMPLES_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Samples/vendor/SDL2-2.0.5/include)
  list(APPEND _CSM_SAMPLES_LIBS SDL2-static SDL2main)


  
  list(APPEND _CSM_SAMPLES_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Samples/vendor/stb)


  if (_CSM_INCLUDE_GLAD)
    list(APPEND _CSM_SAMPLES_INCLUDE_DIRS ${_CSM_GLAD_ROOT_DIR}/include)
    list(APPEND _CSM_SAMPLES_LIBS Glad)
  endif ()


  # Find Core include directory and library to link to.
  list(APPEND _CSM_SAMPLES_INCLUDE_DIRS ${_CSM_CORE_INCLUDE_DIR})


  if (_CSM_BUILD_DEVELOP)
    set(_CSM_CORE_LIB_DIR ${_CSM_CORE_ROOT_DIR}/lib/develop)
  else ()
    set(_CSM_CORE_LIB_DIR ${_CSM_CORE_ROOT_DIR}/lib/release)
  endif ()


  if (WIN64)
    set(_CSM_CORE_LIB_DIR ${_CSM_CORE_LIB_DIR}/windows/x86_64)
  elseif (WIN32)
    set(_CSM_CORE_LIB_DIR ${_CSM_CORE_LIB_DIR}/windows/x86)
  elseif (APPLE)
    set(_CSM_CORE_LIB_DIR ${_CSM_CORE_LIB_DIR}/macos)
  endif ()


  list(APPEND _CSM_SAMPLES_LIBS ${_CSM_CORE_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Live2DCubismCore${CMAKE_STATIC_LIBRARY_SUFFIX})


  # Assemble source files.
  set(_CSM_SAMPLES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Samples/src)


  list(APPEND _CSM_SAMPLES_SOURCE_FILES
    ${_CSM_SAMPLES_SOURCE_DIR}/SampleModel.c

    ${_CSM_SAMPLES_SOURCE_DIR}/Helper/Allocation.c
    ${_CSM_SAMPLES_SOURCE_DIR}/Helper/App.c
    ${_CSM_SAMPLES_SOURCE_DIR}/Helper/stb_image.c)


  if (MSVC)
    list(APPEND _CSM_SAMPLES_SOURCE_FILES ${_CSM_SAMPLES_SOURCE_DIR}/Helper/MsvcAllocation.c)
  else ()
    list(APPEND _CSM_SAMPLES_SOURCE_FILES ${_CSM_SAMPLES_SOURCE_DIR}/Helper/PosixAllocation.c)
  endif ()


  # Assemble defines.
  if (_CSM_USE_GLES20)
    list(APPEND _CSM_SAMPLES_DEFINES -D_CSM_USE_GLES20 -D_CSM_INCLUDE_GLES20)
  endif ()

  if (_CSM_USE_GL33)
    list(APPEND _CSM_SAMPLES_DEFINES -D_CSM_USE_GL33)
  endif()

  if (_CSM_INCLUDE_GLAD)
    list(APPEND _CSM_SAMPLES_DEFINES -D_CSM_INCLUDE_GLAD)
  endif ()


  # Configure 'Hello Cubism'.
  add_executable(HelloCubism
    ${_CSM_SAMPLES_SOURCE_FILES}
    ${_CSM_SAMPLES_SOURCE_DIR}/HelloCubism.c)


  target_include_directories(HelloCubism PRIVATE ${_CSM_SAMPLES_INCLUDE_DIRS})
  target_link_libraries(HelloCubism PRIVATE ${_CSM_SAMPLES_LIBS})
  target_compile_definitions(HelloCubism PRIVATE ${_CSM_SAMPLES_DEFINES})
  add_dependencies(HelloCubism Stb)


  # Configure 'Hello Animation'.
  add_executable(HelloAnimation
    ${_CSM_SAMPLES_SOURCE_FILES}
    ${_CSM_SAMPLES_SOURCE_DIR}/HelloAnimation.c)


  target_include_directories(HelloAnimation PRIVATE ${_CSM_SAMPLES_INCLUDE_DIRS})
  target_link_libraries(HelloAnimation PRIVATE ${_CSM_SAMPLES_LIBS})
  target_compile_definitions(HelloAnimation PRIVATE ${_CSM_SAMPLES_DEFINES})
  add_dependencies(HelloAnimation Stb)


  # Copy sample models.
  add_custom_command(TARGET HelloCubism
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/SampleModels $<TARGET_FILE_DIR:HelloCubism>/Assets)
endif ()
