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

#include <stdlib.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

void* AllocateAligned(const unsigned int size, const unsigned int align)
{
  void* address;


  posix_memalign(&address, align, size);


  return address;
}

void DeallocateAligned(void *alignedMemory)
{
  free(alignedMemory);
}
