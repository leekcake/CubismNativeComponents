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
  int totalVertexCount, totalIndexCount, d, i;
  const int* vertexCounts, * indexCounts;
  csmVector2* mappedAttributeBuffer;
  unsigned short* mappedIndexBuffer;
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


  // Create and initialize vertex array.
  MakeGlVertexArrayInPlace(&renderer->VertexArray);
  BindGlVertexArray(&renderer->VertexArray);


  BindGlBuffer(&renderer->Buffers.Positions);
  glVertexAttribPointer(vertexPositionAttributeLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);


  BindGlBuffer(&renderer->Buffers.Uvs);
  glVertexAttribPointer(vertexUvAttributeLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);


  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);


  BindGlBuffer(&renderer->Buffers.Indices);


  // Unbind resources.
  UnbindGlVertexArray(&renderer->VertexArray);
  UnbindGlBuffer(&renderer->Buffers.Uvs);
  UnbindGlBuffer(&renderer->Buffers.Indices);


  // Initialize static buffers.
  renderDrawables = renderer->RenderDrawables;
  
  
  vertexUvs = csmGetDrawableVertexUvs(renderer->Model);
  mappedAttributeBuffer = MapGlBufferForWrite(&renderer->Buffers.Uvs);


  for (d = 0; d < renderer->DrawableCount; ++d)
  {
    memcpy(mappedAttributeBuffer + renderDrawables[d].Vertices.BaseIndex, vertexUvs[d], ToSizeofVertexData(renderDrawables[d].Vertices.Count));
  }


  UnmapGlBuffer(&renderer->Buffers.Uvs);


  // We store all vertices in one large buffer.
  // As 'glDrawElementsBaseVertex()' is pretty new on mobile, we patch vertex indices by hand here.
  indices = csmGetDrawableIndices(renderer->Model);
  mappedIndexBuffer = MapGlBufferForWrite(&renderer->Buffers.Indices);


  for (d = 0; d < renderer->DrawableCount; ++d)
  {
    for (i = 0; i < renderDrawables[d].Indices.Count; ++i)
    {
      mappedIndexBuffer[i + renderDrawables[d].Indices.BaseIndex] = indices[d][i] + renderDrawables[d].Vertices.BaseIndex; 
    }
  }


  UnmapGlBuffer(&renderer->Buffers.Indices);
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
	ReleaseGlVertexArray(&renderer->VertexArray);
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
  mappedAttributeBuffer = MapGlBufferForWrite(&renderer->Buffers.Positions);


  for (d = 0; d < renderer->DrawableCount; ++d)
  {
    // Update 'inexpensive' data without checking flags.
    renderDrawables[d].IsVisible = IsBitSet(dynamicFlags[d], csmIsVisible);	
    renderDrawables[d].Opacity = opacities[d];


    // Do expensive updates only if necessary.
    if (IsBitSet(dynamicFlags[d], csmVertexPositionsDidChange))
    {
      memcpy(mappedAttributeBuffer + renderDrawables[d].Vertices.BaseIndex, vertexPositions[d], ToSizeofVertexData(renderDrawables[d].Vertices.Count));
    }


    // Check whether drawables need to be sorted.
    sort = sort || IsBitSet(dynamicFlags[d], csmRenderOrderDidChange);
  }


  UnmapGlBuffer(&renderer->Buffers.Positions);


  // Do sort if necessary.
  if (sort)
  {
    UpdateSortableDrawables(renderer->SortedDrawables, renderer->Model);
  }


  // Unset dynamic flags.
  csmResetDrawableDynamicFlags(renderer->Model);
}
