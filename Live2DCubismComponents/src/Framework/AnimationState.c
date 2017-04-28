/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <Live2DCubismFramework.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

void csmResetAnimationState(csmAnimationState* state)
{
  state->Time = 0.0f;
}

void csmTickAnimationState(csmAnimationState* state, const float timeDelta)
{
  state->Time += timeDelta;
}
