/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */
 
 
#include "Local.h"


// -------- //
// REQUIRES //
// -------- //

#if _WIN32
#else
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#endif

// --------- //
// VARIABLES //
// --------- //

/// OpenGL library.
static void* GlLibrary = 0;


// -------------- //
// IMPLEMENTATION //
// -------------- //

GLboolean LoadGl()
{
#if _CSM_INCLUDE_GLAD
  return (gladLoadGL())
    ? GL_TRUE
    : GL_FALSE;
#else
  return GL_TRUE;
#endif
}

void* LoadGlExtension(const char* symbol)
{
#if _WIN32
  return 0;
#else
  char symbolWithoutExtension[256];
  size_t symbolLength;
  void* extension;


  if (!GlLibrary)
  {
#if _CSM_USE_GLES20
    GlLibrary = dlopen("libGLESv2.so", RTLD_LAZY);
#endif


    if (!GlLibrary)
    {
      return 0;
    }
  }


  extension = dlsym(GlLibrary, symbol);


  // Try to load function without 'extension extension'.
  if (!extension)
  {
    // Strip extension identifier.
    symbolLength = strnlen(symbol, 256);


    snprintf(symbolWithoutExtension, symbolLength, "%s", symbol);


    symbolWithoutExtension[symbolLength - 3] = '\0';


    // Try to load symbol again.
    extension = dlsym(GlLibrary, symbolWithoutExtension);
  }


  return extension;
#endif
}
