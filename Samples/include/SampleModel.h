/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#pragma once


// -------- //
// REQUIRES //
// -------- //

#include <Live2DCubismCore.h>
#include <Live2DCubismFramework.h>
#include <Live2DCubismRendering.h>

// Unsigned OpenGL integer.
typedef unsigned int GLuint;

// Single-precision OpenGL float.
typedef float GLfloat;


// ----- //
// TYPES //
// ----- //

enum
{
  /// Number of animations a sample model can contain.
  SampleModelAnimationCapacity = 2
};


/// Encapsulates Cubism model data..
typedef struct SampleModel
{
  /// Revived moc.
  struct csmMoc* Moc;

  /// Moc instance.
  struct csmModel* Model;


  /// Renderer.
  csmGlRenderer* Renderer;

  /// Animation.
  csmAnimation* Animations[SampleModelAnimationCapacity];

  /// Sink for animation.
  csmFloatSink* Sink;
  

  /// Model-view-projection matrix.
  const GLfloat* Mvp;

  /// Model texture.
  GLuint Texture;
}
SampleModel;


// ------------ //
// SAMPLE MODEL //
// ------------ //

/// Loads a Cubism sample model.
///
/// @param  mocPath          Path to serialized model.
/// @param  pngPath          Path to PNG texture.
/// @param  motionJsonPaths  [Optional] Paths to motion.json files.
///
/// @return  Loaded model.
SampleModel LoadModel(const char* mocPath, const char* pngPath, const char** motionJsonPaths);

/// Releases a Cubism sample model.
///
/// @param  model  Model to release.
void ReleaseModel(SampleModel* model);
