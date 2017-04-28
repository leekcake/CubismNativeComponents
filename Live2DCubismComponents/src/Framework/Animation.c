/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <Live2DCubismFramework.h>
#include <Live2DCubismFrameworkInternal.h>


// -------- //
// REQUIRES //
// -------- //

#include "Local.h"

#include <stdio.h>


// --------- //
// FUNCTIONS //
// --------- //

/// Evaluates a linear segment.
///
/// @param  points  Points making up segment.
/// @param  time    Time to evaluate at.
///
/// @return  Value at time.
static float EvaluateLinearSegment(csmAnimationPoint* points, float time)
{
  float t = (time - points[0].Time) / (points[1].Time - points[0].Time);


  return points[0].Value + ((points[1].Value - points[0].Value) * t);
}

/// Evaluates a bézier segment.
///
/// @param  points  Points making up segment.
/// @param  time    Time to evaluate at.
///
/// @return  Value at time.
// TODO Implement bézier evaluation!
static float EvaluateBezierSegment(csmAnimationPoint* points, float time)
{
  float t = (time - points[0].Time) / (points[3].Time - points[0].Time);


  return points[0].Value + ((points[3].Value - points[0].Value) * t);
}


// -------------- //
// IMPLEMENTATION //
// -------------- //

unsigned int csmGetDeserializedSizeofAnimation(const char* motionJson)
{
  unsigned int sizeofAnimation;
  MotionJsonMeta meta;


  ReadMotionJsonMeta(motionJson, &meta);


  return (unsigned int)(sizeof(csmAnimation)
    + (sizeof(csmAnimationCurve) * meta.CurveCount)
    + (sizeof(csmAnimationSegment) * meta.TotalSegmentCount)
    + (sizeof(csmAnimationPoint) * meta.TotalPointCount));
}


void csmResetAnimation(csmAnimation* animation,
                       float duration,
                       short loop,
                       csmAnimationCurve* curves, const short curveCount,
                       csmAnimationSegment* segments,
                       csmAnimationPoint* points)
{
  animation->Duration = duration;
  animation->Loop = loop;

  animation->CurveCount = curveCount;

  animation->Curves = curves;
  animation->Segments = segments;
  animation->Points = points;
}


csmAnimation* csmDeserializeAnimationInPlace(const char *motionJson, void* address, const unsigned int size)
{
  csmAnimation* animation;


  // 'Patch' pointer.
  animation = (csmAnimation*)address;


  // Deserialize animation.
  ReadMotionJson(motionJson, animation);


  return animation;
}


void csmEvaluateAnimation(const csmAnimation *animation,
                          const csmAnimationState *state,
                          const csmFloatBlendFunction blend,
                          const float weight,
                          csmFloatSink *sink)
{
  csmAnimationPoint* basePoint, * nextBasePoint;
  csmAnimationSegment* segment, * lastSegment;
  csmAnimationCurve* curve;
  float time, value;
  int v;


  time = state->Time;


  // 'Repeat' time as necessary.
  if (animation->Loop)
  {
    while (time > animation->Duration)
    {
      time -= animation->Duration;
    }
  }


  // Evaluate curves.
  for (curve = animation->Curves; curve < (animation->Curves + animation->CurveCount); ++curve)
  {
    // Find corresponding value in sink.
    v = csmGetIndexofFloatSinkValue(sink, curve->Type, curve->Id);


    // Skip curve evaluation if no value in sink.
    if (v == -1)
    {
      continue;
    }


    // Find segment to evaluate.
    segment = animation->Segments + curve->BaseSegmentIndex;
    lastSegment = segment + curve->SegmentCount - 1;


    for (; segment < lastSegment; ++segment)
    {
      // Get first point of next segment.
      nextBasePoint = animation->Points + (segment + 1)->BasePointIndex;


      // Break if time lies within current segment.
      if (nextBasePoint->Time > time)
      {
        break;
      }
    }


    // Evaluate segment.
    value = segment->Evaluate(animation->Points + segment->BasePointIndex, time);


    // Blend value into sink.
    sink->Values[v].IsDirty = 1;
    sink->Values[v].Value = blend(sink->Values[v].Value, value, weight);
  }
}
