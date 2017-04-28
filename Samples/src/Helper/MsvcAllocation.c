/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "Allocation.h"


// -------- //
// REQUIRES //
// -------- //

#include <malloc.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

void* AllocateAligned(const unsigned int size, const unsigned int align)
{
  return _aligned_malloc(size, align);
}

void DeallocateAligned(void *alignedMemory)
{
  _aligned_free(alignedMemory);
}
