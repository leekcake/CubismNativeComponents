# ---- #
# META #
# ---- #

cmake_minimum_required(VERSION 3.6)


# ------------ #
# PUBLIC LISTS #
# ------------ #

get_directory_property(_CSM_COMPONENTS_PARENT_SCOPE PARENT_DIRECTORY)
if(_CSM_COMPONENTS_PARENT_SCOPE)
  # Will contain include directory.
  set(CSM_COMPONENTS_INCLUDE_DIR "" PARENT_SCOPE)

  # Will contain target link libraries (excluding Core).
  set(CSM_COMPONENTS_LIBS "" PARENT_SCOPE)

  # Will contain target dependencies.
  set(CSM_COMPONENTS_DEPS "" PARENT_SCOPE)
endif ()


# ------------ #
# USER OPTIONS #
# ------------ #

# Path to Cubism Core include directory.
set(CSM_COMPONENTS_CORE_INCLUDE_DIRECTORY "../Core/include" CACHE STRING "Path to Live2D Cubism Core include directory for native development.")

# Enables OpenGL reference implementation.
option(CSM_COMPONENTS_BUILD_GL_RENDERER "Enables OpenGL reference renderer." OFF)

# Path to OpenGL header on desktop.
if (NOT ANDROID AND NOT EMSCRIPTEN AND NOT IOS AND NOT RPI)
  set(CSM_COMPONENTS_GL_H "" CACHE STRING "Path to OpenGL header.")
endif ()



# ----------------------- #
# OPTIONS INTERNALIZATION #
# ----------------------- #

# Internalize Core path.
if (NOT IS_ABSOLUTE ${CSM_COMPONENTS_CORE_INCLUDE_DIRECTORY})
  set(_CSM_COMPONENTS_CORE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/${CSM_COMPONENTS_CORE_INCLUDE_DIRECTORY})
else ()
  set(_CSM_COMPONENTS_CORE_INCLUDE_DIR ${CSM_COMPONENTS_CORE_INCLUDE_DIRECTORY})
endif ()

# Internalize renderer option.
if (CSM_COMPONENTS_BUILD_GL_RENDERER)
  set(_CSM_COMPONENTS_BUILD_GL_RENDERER ON)
endif ()


# Detect desktop.
if (NOT ANDROID AND NOT EMSCRIPTEN AND NOT IOS AND NOT RPI)
  set(_CSM_COMPONENTS_DESKTOP ON)
endif ()


# Internalize OpenGL header.
if (_CSM_COMPONENTS_BUILD_GL_RENDERER AND _CSM_COMPONENTS_DESKTOP)
  if (NOT IS_ABSOLUTE ${CSM_COMPONENTS_GL_H})
    set(_CSM_COMPONENTS_GL_H ${CMAKE_CURRENT_LIST_DIR}/${CSM_COMPONENTS_GL_H})
  else ()
    set(_CSM_COMPONENTS_GL_H ${CSM_COMPONENTS_GL_H})
  endif ()
endif ()


# Internalize OpenGL version.
if (_CSM_COMPONENTS_DESKTOP)
  set(_CSM_COMPONENTS_USE_GL33 ON)
else ()
  set(_CSM_COMPONENTS_USE_GLES20 ON)
endif ()


# -------------------- #
# OPTIONS SANITIZATION #
# -------------------- #

# Make sure Core include directory is valid.
if (NOT EXISTS "${_CSM_COMPONENTS_CORE_INCLUDE_DIR}/Live2DCubismCore.h")
  message(FATAL_ERROR "[Live2D Cubism Components] Live2D Cubism Core header not found.")
endif ()

# Make sure OpenGL header is valid if required.
if (_CSM_COMPONENTS_GL_H AND (NOT EXISTS ${_CSM_COMPONENTS_GL_H} OR IS_DIRECTORY ${_CSM_COMPONENTS_GL_H}))
  message(FATAL_ERROR "[Live2D Cubism Components] OpenGL header not found.")
endif ()


# ---------- #
# COMPONENTS #
# ---------- #

# Set include directories.
set(_CSM_COMPONENTS_INCLUDE_DIRS
  ${_CSM_COMPONENTS_CORE_INCLUDE_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/include)


# Set source files.
set (_CSM_COMPONENTS_SRC_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/Logging.c
  
  
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/Animation.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/AnimationSegmentEvaluationFunction.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/AnimationState.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/AnimationUserDataCallback.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/FloatBlendFunction.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/Json.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/ModelExtensions.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/MotionJson.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/String.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/PhysicsMath.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/PhysicsJson.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/Physics.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/UserData.c
  ${CMAKE_CURRENT_LIST_DIR}/src/Framework/UserDataJson.c)


if (_CSM_COMPONENTS_BUILD_GL_RENDERER)
  list(APPEND _CSM_COMPONENTS_SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/src/Rendering/GlBuffer.c
    ${CMAKE_CURRENT_LIST_DIR}/src/Rendering/GlDraw.c
    ${CMAKE_CURRENT_LIST_DIR}/src/Rendering/GlMaskbuffer.c
    ${CMAKE_CURRENT_LIST_DIR}/src/Rendering/GlProgram.c
    ${CMAKE_CURRENT_LIST_DIR}/src/Rendering/GlRenderer.c
    ${CMAKE_CURRENT_LIST_DIR}/src/Rendering/RenderDrawable.c
    ${CMAKE_CURRENT_LIST_DIR}/src/Rendering/SortableDrawable.c)
endif ()


# Set defines.
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(_CSM_COMPONENTS_DEFINES "")
else ()
  set(_CSM_COMPONENTS_DEFINES -D_CSM_COMPONENTS_BUILD_DEVELOP)
endif ()


if (_CSM_COMPONENTS_BUILD_GL_RENDERER)
  if (_CSM_COMPONENTS_GL_H)
    list(APPEND _CSM_COMPONENTS_DEFINES -D_CSM_COMPONENTS_GL_H="${_CSM_COMPONENTS_GL_H}")
  endif ()


  if (_CSM_COMPONENTS_USE_GL33)
    list(APPEND _CSM_COMPONENTS_DEFINES -D_CSM_COMPONENTS_USE_GL33)
  else ()
    list(APPEND _CSM_COMPONENTS_DEFINES -D_CSM_COMPONENTS_USE_GLES20)
  endif ()
endif ()


# Configure library.
add_library(Live2DCubismComponents STATIC ${_CSM_COMPONENTS_SRC_FILES})


target_compile_definitions(Live2DCubismComponents PRIVATE ${_CSM_COMPONENTS_DEFINES})
target_include_directories(Live2DCubismComponents PRIVATE ${_CSM_COMPONENTS_INCLUDE_DIRS})


# Set public lists.
if(_CSM_COMPONENTS_PARENT_SCOPE)
  set(CSM_COMPONENTS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include PARENT_SCOPE)


  set(CSM_COMPONENTS_LIBS Live2DCubismComponents PARENT_SCOPE)


  if (_CSM_COMPONENTS_BUILD_GL_RENDERER AND NOT WIN32)
    if (_CSM_COMPONENTS_USE_GLES20)
      list(APPEND CSM_COMPONENTS_LIBS GLESv2)
    endif ()
  endif ()


  set(CSM_COMPONENTS_DEPS Live2DCubismComponents PARENT_SCOPE)
endif ()
