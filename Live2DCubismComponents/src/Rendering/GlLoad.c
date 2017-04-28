/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */
 
 
 #include "Local.h"


// -------------- //
// IMPLEMENTATION //
// -------------- //

GLboolean LoadGl()
{
#if _CSM_INCLUDE_GLAD
  return (gladLoadGL())
    ? GL_TRUE
    : GL_FALSE;
#else
  return GL_TRUE;
#endif
}
