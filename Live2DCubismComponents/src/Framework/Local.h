/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#ifndef _CSM_LOCAL_H
#define _CSM_LOCAL_H


// -------- //
// REQUIRES //
// -------- //

typedef struct csmAnimation csmAnimation;


// ----- //
// TYPES //
// ----- //

/// Meta data of a serialized motion.
typedef struct MotionJsonMeta
{
  /// Duration of motion in seconds.
  float Duration;

  /// Frames per second.
  float Fps;

  /// Non-zero if motion should loop.
  int Loop;


  /// Number of curves serialized motion contains.
  int CurveCount;

  /// Total number of segments motion contains.
  int TotalSegmentCount;

  /// Total number of points motion contains.
  int TotalPointCount;
}
MotionJsonMeta;


// ------ //
// STRING //
// ------ //

/// Reads a float from a string.
///
/// @param  string  String to read from.
/// @param  buffer  Buffer to write to.
void ReadFloatFromString(const char* string, float* buffer);

/// Reads an integer from a string.
///
/// @param  string  String to read from.
/// @param  buffer  Buffer to write to.
void ReadIntFromString(const char* string, int* buffer);


/// Checks whether a string begins with a given value.
///
/// @param  string    String to check.
/// @param  expected  Value to match against.
///
/// @return Non-zero if expectation met; '0' otherwise.
int DoesStringStartWith(const char* string, const char* expected);


// ----------- //
// MOTION JSON //
// ----------- //

/// Reads serialized motion meta.
///
/// @param  motionJson  Motion JSON string.
/// @param  buffer      Buffer to read into.
void ReadMotionJsonMeta(const char* motionJson, MotionJsonMeta* buffer);

/// Reads a serialized motion.
///
/// @param  motionJson  Motion JSON string.
/// @param  buffer      Buffer to read into.
void ReadMotionJson(const char* motionJson, csmAnimation* buffer);


#endif
