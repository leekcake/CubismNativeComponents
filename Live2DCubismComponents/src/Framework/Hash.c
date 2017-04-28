/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <Live2DCubismFrameworkInternal.h>


// --------- //
// FUNCTIONS //
// --------- //

/// Computes length of a string by looking for its null-terminator.
///
/// @param  string      Null-terminated string to measure.
/// @param  iterations  Maximum number of iterations.
///
/// @return  Length of string (or iterations).
int GetStringLength(const char* string, const int iterations)
{
  int c;


  for (c = 0; c < iterations && string[c] != '\0'; ++c)
  {
    ;
  }


  return c;
}


// -------------- //
// IMPLEMENTATION //
// -------------- //

csmHash csmHashString(const char* string)
{
  return csmHashSubString(string, 0, GetStringLength(string, 64));
}

// INV  Make sure algorithm is sufficient for its purpose.
csmHash csmHashSubString(const char* string, const int begin, const int end)
{
  csmHash hash;
  int c;


  for (hash = 0, c = begin; c < end; ++c)
  {
    hash = (hash * 13) + (unsigned char)string[c];
  }


  return hash;
}
