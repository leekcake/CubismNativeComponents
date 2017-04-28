/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <Live2DCubismRendering.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

csmGlVersion csmGetRequiredGlVersion()
{
	#if _CSM_USE_GL33
		return csmGl33;
	#elif _CSM_USE_GLES20
		return csmGles20;
	#endif
}
