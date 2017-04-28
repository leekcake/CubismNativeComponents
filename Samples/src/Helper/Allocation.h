/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#pragma once


// ---------- //
// ALLOCAITON //
// ---------- //

/// Allocates aligned heap memory.
///
/// @param  size   Number of bytes to allocate.
/// @param  align  Alignment for memory block.
///
/// @return  Valid address to allocated memory on success; '0' otherwise.
void* AllocateAligned(const unsigned int size, const unsigned int align);

/// Frees aligned heap memory.
///
/// @param  memory  Address of memory block to free.
void DeallocateAligned(void* alignedMemory);


/// Allocates heap memory without user align constraint.
///
/// @param  size   Number of bytes to allocate.
///
/// @return  Valid address to allocated memory on success; '0' otherwise.
void* Allocate(const unsigned int size);

/// Frees non-user alignment constraint heap memory.
///
/// @param  memory  Address of memory block to free.
void Deallocate(void* Memory);
