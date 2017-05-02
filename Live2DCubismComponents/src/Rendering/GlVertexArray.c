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

#include <Live2DCubismRenderingInternal.h>


// --------------------- //
// OPENGLES 2 EXTENSIONS //
// --------------------- //

#if _CSM_USE_GLES20

/// Wraps 'glBindVertexArray'.
///
/// @param  array  Vertex array to bind.
static void glBindVertexArray(GLuint array)
{
  static PFNGLBINDVERTEXARRAYOESPROC proc = 0;


  if (!proc)
  {
    proc = LoadGlExtension("glBindVertexArrayOES");
  }


  proc(array);
}


/// Wraps 'glDeleteVertexArrays'.
///
/// @param  n       Number of arrays to delete.
/// @param  arrays  Arrays to delete.
static void glDeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
  static PFNGLDELETEVERTEXARRAYSOESPROC proc = 0;


  if (!proc)
  {
    proc = LoadGlExtension("glDeleteVertexArraysOES");
  }


   proc(n, arrays);
}

/// Wraps 'glDeleteVertexArrays'.
///
/// @param  n       Number of arrays to delete.
/// @param  arrays  Arrays to delete.
static void glGenVertexArrays(GLsizei n, GLuint *arrays)
{
  static PFNGLGENVERTEXARRAYSOESPROC proc = 0;


  if (!proc)
  {
    proc = LoadGlExtension("glGenVertexArraysOES");
  }


   proc(n, arrays);
}

#endif


// -------------- //
// IMPLEMENTATION //
// -------------- //

void MakeGlVertexArrayInPlace(GLuint* array)
{
  glGenVertexArrays(1, array);
}

void ReleaseGlVertexArray(GLuint* array)
{
  glDeleteVertexArrays(1, array);
}


void BindGlVertexArray(const GLuint* array)
{
  glBindVertexArray(*array);
}

void UnbindGlVertexArray(const GLuint* array)
{
  glBindVertexArray(0);
}
