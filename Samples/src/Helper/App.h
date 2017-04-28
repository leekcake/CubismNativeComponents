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

// Single-precision OpenGL float.
typedef float GLfloat;


// --- //
// APP //
// --- //

/// Gets the app view projection matrix.
///
/// @param  4x4 OpenGL matrix.
const GLfloat* GetViewProjectionMatrix();
