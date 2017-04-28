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

#include "Local.h"

#include <Live2DCubismCore.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

int csmGetIndexofParameter(const csmModel* model, const char* id)
{
  int p;


  for (p = 0; p < csmGetParameterCount(model); ++p)
  {
    if (!DoesStringStartWith(csmGetParameterIds(model)[p], id))
    {
      continue;
    }


    return p;
  }


  return -1;
}
