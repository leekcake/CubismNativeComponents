/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <Live2DCubismFramework.h>


// -------- //
// REQUIRES //
// -------- //

#include <Live2DCubismCore.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

int csmDoesModelUseMasks(const csmModel* model)
{
  int d;


  for (d = 0; d < csmGetDrawableCount(model); ++d)
  {
    if (csmGetDrawableMaskCounts(model)[d] <= 0)
    {
      continue;
    }


    return 1;
  }


  return 0;
}
