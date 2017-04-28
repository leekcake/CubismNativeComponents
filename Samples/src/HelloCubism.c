/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "Helper/ISample.h"


// ----------- //
// DESCRIPTION //
// ----------- //

// TODO Add.


// -------- //
// REQUIRES //
// -------- //

#include <SampleModel.h>


// --------- //
// VARIABLES //
// --------- //


/// Sample model.
static SampleModel MaskKun = {0};

/// Index of parameter to adjust.
static int TargetParameterIndex = 1;

/// Direction to move parameter by.
static float Direction = 50.0f;


// -------------- //
// IMPLEMENTATION //
// -------------- //

void OnDidStart()
{
  // Load model.
  MaskKun = LoadModel("Assets/MaskKun/Clipping.moc3", "Assets/MaskKun/Clipping.png", 0);


  // Find parameter.
  TargetParameterIndex = csmGetIndexofParameter(MaskKun.Model, "PARAM_ANGLE_X");
}


void OnTick(float deltaTime)
{
  float minimumValue, maximumValue, * value;
  int p;


  p = TargetParameterIndex;


  // Get parameter limits.
  minimumValue = csmGetParameterMinimumValues(MaskKun.Model)[p];
  maximumValue = csmGetParameterMaximumValues(MaskKun.Model)[p];


  // Update parameter value.
  value = &csmGetParameterValues(MaskKun.Model)[p];


  (*value) += (deltaTime * Direction);


  // Make sure head motion loops.
  if (Direction < 0.0f && (*value) < minimumValue)
  {
    Direction = 50.0f;
  }
  else if (Direction > 0.0f && (*value) > maximumValue)
  {
    Direction = -50.0f;
  }


  // Update model and renderer.
  csmUpdateModel(MaskKun.Model);
  csmUpdateGlRenderer(MaskKun.Renderer);


  // Render model.
  csmGlDraw(MaskKun.Renderer, MaskKun.Mvp, &MaskKun.Texture);
}


void OnWillQuit()
{
  // Unload model.
  ReleaseModel(&MaskKun);
}
