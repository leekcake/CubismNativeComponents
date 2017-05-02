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

/// Matches 'GL_WRITE_ONLY_OES'.
static const GLenum GL_WRITE_ONLY = 0x88B9;


/// Wraps 'glMapBuffer'.
///
/// @param  target  Target type to map.
/// @param  access  Access type.
///
/// @return  Valid pointer to mapped buffer on success; '0' otherwise.
static void* glMapBuffer(GLenum target, GLenum access)
{
  static PFNGLMAPBUFFEROESPROC proc = 0;


  if (!proc)
  {
    proc = LoadGlExtension("glMapBufferOES");
  }


  return proc(target, access);
}

/// Wraps 'glUnmapBuffer'.
///
/// @param  target  Target type to unmap.
///
/// @return 'GL_TRUE' unless the data store contents have become corrupt during the time the data store was mapped.
static GLboolean glUnmapBuffer(GLenum target)
{
  static PFNGLUNMAPBUFFEROESPROC proc = 0;


  if (!proc)
  {
    proc = LoadGlExtension("glUnmapBufferOES");
  }


  return proc(target);
}

#endif


// -------------- //
// IMPLEMENTATION //
// -------------- //

void MakeStaticGlBufferInPlace(GlBuffer* buffer, const GLenum type, const GLsizeiptr size)
{
  // Store type.
  buffer->Type = type;


  // Generate OpenGL resource.
  glGenBuffers(1, &buffer->Handle);
  glBindBuffer(type, buffer->Handle);
  glBufferData(type, size, 0, GL_STATIC_DRAW);
  glBindBuffer(type, 0);
}

void MakeDynamicGlBufferInPlace(GlBuffer* buffer, const GLenum type, const GLsizeiptr size)
{
  // Store type.
  buffer->Type = type;


  // Generate OpenGL resource.
  glGenBuffers(1, &buffer->Handle);
  glBindBuffer(type, buffer->Handle);
  glBufferData(type, size, 0, GL_DYNAMIC_DRAW);
  glBindBuffer(type, 0);
}

void ReleaseGlBuffer(GlBuffer* buffer)
{
  // Release OpenGL resource.
  glDeleteBuffers(1, &buffer->Handle);


  // Reset fields.
  buffer->Type = 0;
  buffer->Handle = 0;
}


void BindGlBuffer(GlBuffer* buffer)
{
  glBindBuffer(buffer->Type, buffer->Handle);
}

void UnbindGlBuffer(GlBuffer* buffer)
{
  glBindBuffer(buffer->Type, 0);
}


void* MapGlBufferForWrite(GlBuffer* buffer)
{
  glBindBuffer(buffer->Type, buffer->Handle);


  return glMapBuffer(buffer->Type, GL_WRITE_ONLY);
}

void UnmapGlBuffer(GlBuffer* buffer)
{
  glUnmapBuffer(buffer->Type);
  glBindBuffer(buffer->Type, 0);
}
