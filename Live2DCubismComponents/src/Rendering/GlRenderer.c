/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <Live2DCubismRendering.h>


// -------- //
// REQUIRES //
// -------- //

#include "Local.h"

#include <Live2DCubismCore.h>
#include <Live2DCubismRenderingInternal.h>

#include <string.h>


// --------- //
// CONSTANTS //
// --------- //
enum { TemporaryBufferLength = 128 };


// --------- //
// FUNCTIONS //
// --------- //

// Converts from vertex data count to byte count.
//
// @param  count  Vertex data count.
//
// @return  Size of vertex data in bytes.
static inline GLsizeiptr ToSizeofVertexData(const unsigned short count)
{
  return sizeof(csmVector2) * count;
}

// Converts from index data count to byte count.
//
// @param  count  Index data count.
//
// @return  Size of index data in bytes.
static inline GLsizeiptr ToSizeofIndexData(const unsigned short count)
{
  return sizeof(unsigned short) * count;
}


/// Creates and initializes OpenGL buffers and vertex array.
/// Make sure to call this function AFTER non-OpenGL related renderer fields are initialized.
///
/// @param  renderer                         Renderer to initialize.
/// @param  vertexPositionAttributeLocation  Attribute location for the vertex positions.
/// @param  vertexUvAttributeLocation        Attribute location for the vertex uvs.
static void InitializeBuffersAndVertexArray(csmGlRenderer* renderer,
                                            const GLint vertexPositionAttributeLocation,
                                            const GLint vertexUvAttributeLocation)
{
  int totalVertexCount, totalIndexCount, d, i, j;
  const int* vertexCounts, * indexCounts;
  unsigned short temporaryIndexBuffer[TemporaryBufferLength];
  RenderDrawable* renderDrawables;
  const unsigned short** indices;
  const csmVector2** vertexUvs;


  // Count model vertices and indices.
  totalVertexCount = 0;
  totalIndexCount = 0;


  vertexCounts = csmGetDrawableVertexCounts(renderer->Model);
  indexCounts = csmGetDrawableIndexCounts(renderer->Model);


  for (d = 0; d < renderer->DrawableCount; ++d)
  {
	  totalVertexCount += vertexCounts[d];
	  totalIndexCount += indexCounts[d];
  }


  // Create buffers.
  MakeDynamicGlBufferInPlace(&renderer->Buffers.Positions, GL_ARRAY_BUFFER, ToSizeofVertexData(totalVertexCount));
  MakeStaticGlBufferInPlace(&renderer->Buffers.Uvs, GL_ARRAY_BUFFER, ToSizeofVertexData(totalVertexCount));
  MakeStaticGlBufferInPlace(&renderer->Buffers.Indices, GL_ELEMENT_ARRAY_BUFFER, ToSizeofIndexData(totalIndexCount));


  // Initialize static buffers.
  renderDrawables = renderer->RenderDrawables;
  
  
  vertexUvs = csmGetDrawableVertexUvs(renderer->Model);
  BindGlBuffer(&renderer->Buffers.Uvs);


  for (d = 0; d < renderer->DrawableCount; ++d)
  {
    WriteToGlBuffer(&renderer->Buffers.Uvs, ToSizeofVertexData(renderDrawables[d].Vertices.BaseIndex), ToSizeofVertexData(renderDrawables[d].Vertices.Count), vertexUvs[d]);
  }


  UnbindGlBuffer(&renderer->Buffers.Uvs);


  // We store all vertices in one large buffer.
  // As 'glDrawElementsBaseVertex()' is pretty new on mobile, we patch vertex indices by hand here.
  indices = csmGetDrawableIndices(renderer->Model);
  BindGlBuffer(&renderer->Buffers.Indices);


  i = 0;
  j = 0;


  for (d = 0; d < renderer->DrawableCount; ++d)
  {
   Loop:

    while (j < TemporaryBufferLength)
    {
      if (i >= renderDrawables[d].Indices.Count)
      {
        break;
      }
      

      temporaryIndexBuffer[j] = indices[d][i] + renderDrawables[d].Vertices.BaseIndex;;


      ++j;
      ++i;
    }


    WriteToGlBuffer(&renderer->Buffers.Indices, ToSizeofIndexData((i - j) + renderDrawables[d].Indices.BaseIndex), ToSizeofIndexData(j), temporaryIndexBuffer);
    j = 0;


    if (i < renderDrawables[d].Indices.Count)
    {
      goto Loop;
    }


    i = 0;
  }


  UnbindGlBuffer(&renderer->Buffers.Indices);
}


// -------------- //
// IMPLEMENTATION //
// -------------- //

unsigned int csmGetSizeofGlRenderer(const csmModel* model)
{
	return (unsigned int)(sizeof(csmGlRenderer) + ((sizeof(csmRenderDrawable) + sizeof(csmSortableDrawable)) * csmGetDrawableCount(model)));
}


csmGlRenderer* csmMakeGlRendererInPlace(csmModel* model,
										                    void* address,
										                    const unsigned int size)
{
  csmGlRenderer* renderer;


  // Acquire resources.
  RequireGlPrograms();
  RequireGlMaskbuffer();


  // First initialize as barebone...
  renderer = csmMakeBareboneGlRendererInPlace(model,
                                              address,
                                              size,
                                              GetGlVertexPositionLocation(),
                                              GetGlVertexUvLocation());


  // ... then flag as non-barebone.
  renderer->IsBarebone = 0;


  return renderer;
}

csmGlRenderer* csmMakeBareboneGlRendererInPlace(csmModel* model,
						  							                  	void* address,
			                                        	const unsigned int size,
																								const GLint vertexPositionAttributeLocation,
																								const GLint vertexUvAttributeLocation)
{
	csmGlRenderer* renderer = (csmGlRenderer*)address;


	// Make sure loader is initialized.
	LoadGl();


  // Initialize non-OpenGL related fields.
  renderer->DrawableCount = csmGetDrawableCount(model);
  renderer->RenderDrawables = (csmRenderDrawable*)(renderer + 1);
  renderer->SortedDrawables = (csmSortableDrawable*)(renderer->RenderDrawables + renderer->DrawableCount);
  renderer->Model = model;


  InitializeRenderDrawables(renderer->RenderDrawables, model);
  InitializeSortableDrawables(renderer->SortedDrawables, model);


  // Initialize OpenGL resources and related drawables.
  InitializeBuffersAndVertexArray(renderer, vertexPositionAttributeLocation, vertexUvAttributeLocation);


	// Finalize initialization by calling update once.
	csmUpdateGlRenderer(renderer);


	// Return on success.
	return renderer;
}

void csmReleaseGlRenderer(csmGlRenderer* renderer)
{
	// Release GL resources.
	ReleaseGlBuffer(&renderer->Buffers.Indices);
	ReleaseGlBuffer(&renderer->Buffers.Uvs);
	ReleaseGlBuffer(&renderer->Buffers.Positions);


  // Release draw-resources unless barebone.
  if (!renderer->IsBarebone)
  {
    UnrequireGlMaskbuffer();
    UnrequireGlPrograms();
  }
}


void csmUpdateGlRenderer(csmGlRenderer* renderer)
{
  const csmVector2** vertexPositions;
  csmVector2* mappedAttributeBuffer;
  const unsigned char* dynamicFlags;
  RenderDrawable* renderDrawables;
  const float* opacities;
  int d, sort;


  // Initialize locals.
  vertexPositions = csmGetDrawableVertexPositions(renderer->Model);
  dynamicFlags = csmGetDrawableDynamicFlags(renderer->Model);
  opacities = csmGetDrawableOpacities(renderer->Model);

  renderDrawables = renderer->RenderDrawables;

  sort = 0;


  // Fetch dynamic data.
  BindGlBuffer(&renderer->Buffers.Positions);


  for (d = 0; d < renderer->DrawableCount; ++d)
  {
    // Update 'inexpensive' data without checking flags.
    renderDrawables[d].IsVisible = IsBitSet(dynamicFlags[d], csmIsVisible);	
    renderDrawables[d].Opacity = opacities[d];


    // Do expensive updates only if necessary.
    if (IsBitSet(dynamicFlags[d], csmVertexPositionsDidChange))
    {
      WriteToGlBuffer(&renderer->Buffers.Positions, ToSizeofVertexData(renderDrawables[d].Vertices.BaseIndex), ToSizeofVertexData(renderDrawables[d].Vertices.Count), vertexPositions[d]);
    }


    // Check whether drawables need to be sorted.
    sort = sort || IsBitSet(dynamicFlags[d], csmRenderOrderDidChange);
  }


  UnbindGlBuffer(&renderer->Buffers.Positions);


  // Do sort if necessary.
  if (sort)
  {
    UpdateSortableDrawables(renderer->SortedDrawables, renderer->Model);
  }


  // Unset dynamic flags.
  csmResetDrawableDynamicFlags(renderer->Model);
}
