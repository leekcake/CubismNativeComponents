/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#pragma once


// ----- //
// TYPES //
// ----- //

/// 16-bit hash value.
typedef unsigned short csmHash;


/// JSON token type.
typedef enum csmJsonTokenType
{
  /// Begin of object.
  csmJsonObjectBegin,

  /// End of object.
  csmJsonObjectEnd,


  /// Begin of array.
  csmJsonArrayBegin,

  /// End of array.
  csmJsonArrayEnd,


  /// Key.
  csmJsonName,


  /// String value.
  csmJsonString,

  /// Number value.
  csmJsonNumber,

  /// 'true' value.
  csmJsonTrue,

  /// 'false' value.
  csmJsonFalse,

  /// 'null' value.
  csmJsonNull,


  /// Number of JSON token types.
  // (Make sure this value always is the last value of the enumeration).
  csmJsonTokenTypeCount
}
csmJsonTokenType;


/// JSON token handler when lexing JSONs.
///
/// @param  jsonString  Input JSON string.
/// @param  type        Token type.
/// @param  begin       Begin of token as offset into string (in chars).
/// @param  end         End of token as offset into string (in chars).
/// @param  userData    [Optional] User data.
///
/// @param Non-zero to continue lexing; '0' to stop lexing.
typedef int (*csmJsonTokenHandler)(const char* jsonString,
                                   const csmJsonTokenType type,
                                   const int begin,
                                   const int end,
                                   void* userData);


/// Single point making up an animation curve.
typedef struct csmAnimationPoint
{
  /// Timing of point.
  float Time;

  /// Value at time.
  float Value;
}
csmAnimationPoint;


/// Animation segment evaluation function.
///
/// @param  points  Points making up the segment.
/// @param  time    Time to evaluate at.
///
/// @return  Evaluation result.
typedef float (*csmAnimationSegmentEvaluationFunction)(const csmAnimationPoint* points, const float time);

/// Builtin linear animation segment evaluation.
///
/// First 2 points are evaluated.
float csmLinearAnimationSegmentEvaluationFunction(const csmAnimationPoint* points, const float time);

/// Builtin bezier animation segment evaluation.
///
/// First 4 points are evaluated.
float csmBezierAnimationSegmentEvaluationFunction(const csmAnimationPoint* points, const float time);

/// Builtin stepped animation segment evaluation.
///
/// First 2 points are evaluated.
float csmSteppedAnimationSegmentEvaluationFunction(const csmAnimationPoint* points, const float time);

/// Builtin inverse stepped animation segment evaluation.
///
/// First 2 points are evaluated.
float csmInverseSteppedAnimationSegmentEvaluationFunction(const csmAnimationPoint* points, const float time);


/// Animation curve types.
enum
{
  /// Model animation curve.
  csmModelAnimationCurve,

  /// Parameter animation curve.
  csmParameterAnimationCurve,

  /// Part opacity animation curve.
  csmPartOpacityAnimationCurve
};


/// Single animation curve segment.
typedef struct csmAnimationSegment
{
  /// Allows evaluating segment.
  csmAnimationSegmentEvaluationFunction Evaluate;

  /// Index of first segment point.
  int BasePointIndex;
}
csmAnimationSegment;


/// Animation curve.
typedef struct csmAnimationCurve
{
  /// Curve target type.
  short Type;

  /// ID of curve.
  csmHash Id;

  /// Number of segments the curve contains.
  int SegmentCount;

  /// Index of first segment in curve.
  int BaseSegmentIndex;
}
csmAnimationCurve;


/// Animation.
typedef struct csmAnimation
{
  /// Duration in seconds.
  float Duration;

  /// Non-zero if animation should loop.
  short Loop;


  /// Number of curves.
  short CurveCount;

  /// Curves.
  csmAnimationCurve* Curves;

  /// Curve segments.
  csmAnimationSegment* Segments;

  /// Curve points.
  csmAnimationPoint* Points;
}
csmAnimation;


// ---------------- //
// MODEL EXTENSIONS //
// ---------------- //

/// Hashes part of a string.
///
/// @param  string    String to hash from.
/// @param  idBegin   Inclusive offset into string to start hashing at.
/// @param  idEnd     Exclusive offset into string to stop hashing at.
///
/// @return  Non-zero hash value on success; '0' otherwise.
csmHash csmHashIdFromSubString(const char* string, const int idBegin, const int idEnd);


// ---- //
// JSON //
// ---- //

/// Lexes a JSON string.
///
/// @param  jsonString  JSON string to lex.
/// @param  onToken     Token handler.
/// @param  userData    [Optional] Data to pass to token handler.
void csmLexJson(const char* jsonString, csmJsonTokenHandler onToken, void* userData);


// --------- //
// ANIMATION //
// --------- //

/// Initializes an animation.
///
/// @param  animation     Animation to reset.
/// @param  duration      Duration in seconds.
/// @param  loop          Loop flag.
/// @param  curves        Curve data.
/// @param  curveCount    Number of curves.
/// @param  segments      Segment data.
/// @param  points        Point data.
void csmInitializesAnimation(csmAnimation* animation,
                             float duration,
                             short loop,
                             csmAnimationCurve* curves, const short curveCount,
                             csmAnimationSegment* segments,
                             csmAnimationPoint* points);
