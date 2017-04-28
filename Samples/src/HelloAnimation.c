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

// TODO  Add description.


// -------- //
// REQUIRES //
// -------- //

#include <SampleModel.h>


// ----- //
// TYPES //
// ----- //

/// Encapsulates data making it easy to blend animations.
typedef struct AnimationLayer
{
  /// Animation.
  csmAnimation *Animation;

  /// Animation state.
  csmAnimationState State;

  /// Blend function.
  csmFloatBlendFunction Blend;

  /// Blend weight.
  float Weight;
}
AnimationLayer;


// --------- //
// VARIABLES //
// --------- //

/// Paths to motion files.
static const char* MotionJsonPaths[] =
{
  "Assets/Koharu/Motion.motion3.json",
  "Assets/Koharu/Expression.motion3.json"
};


/// Sample model.
static SampleModel Koharu = {0};


/// Animation layers.
static AnimationLayer AnimationLayers[SampleModelAnimationCapacity] = {0};


// -------------- //
// IMPLEMENTATION //
// -------------- //

void OnDidStart()
{
  int l;


  // Load model.
  Koharu = LoadModel("Assets/Koharu/Koharu.moc3", "Assets/Koharu/Koharu.png", MotionJsonPaths);


  // Initialize animation layers.
  for (l = 0; l < SampleModelAnimationCapacity; ++ l)
  {
    AnimationLayers[l].Animation = Koharu.Animations[l];
    AnimationLayers[l].Blend = csmOverrideFloatBlendFunction;
    AnimationLayers[l].Weight = 1.0f;


    csmResetAnimationState(&AnimationLayers[l].State);
  }
}


void OnTick(float deltaTime)
{
  int l;


  // Update and evaluate animation layers.
  for (l = 0; l < SampleModelAnimationCapacity; ++ l)
  {
    csmTickAnimationState(&AnimationLayers[l].State, deltaTime);


    csmEvaluateAnimation(AnimationLayers[l].Animation,
                         &AnimationLayers[l].State,
                         AnimationLayers[l].Blend,
                         AnimationLayers[l].Weight,
                         Koharu.Sink);
  }


  // Flush sink (without handling model float values).
  csmFlushFloatSink(Koharu.Sink, Koharu.Model, 0, 0);


  // Update model and renderer.
  csmUpdateModel(Koharu.Model);
  csmUpdateGlRenderer(Koharu.Renderer);


  // Draw renderer.
  csmGlDraw(Koharu.Renderer, Koharu.Mvp, &Koharu.Texture);
}


void OnWillQuit()
{
  // Unload model.
  ReleaseModel(&Koharu);
}
