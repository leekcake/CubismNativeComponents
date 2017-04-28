/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <Live2DCubismRendering.h>
#include <Live2DCubismRenderingInternal.h>


// ------------------- //
// STUB IMPLEMENTATION //
// ------------------- //

csmGlVersion csmGetRequiredGlVersion()
{
  return csmGlStub;
}


csmGlRenderer* csmMakeGlRendererInPlace(csmModel* model,
						  				                  void* address,
			                                  const unsigned int size)
{
  return 0;
}

csmGlRenderer* csmMakeBareboneGlRendererInPlace(csmModel* model,
						  				                   			  void* address,
			                                  			  const unsigned int size)
{
  return 0;
}

void csmReleaseGlRenderer(csmGlRenderer* renderer) {}


void csmUpdateGlRenderer(csmGlRenderer* renderer) {}


void csmGlDraw(const csmGlRenderer* renderer, const GLfloat* mvp, const GLuint* textures) {}