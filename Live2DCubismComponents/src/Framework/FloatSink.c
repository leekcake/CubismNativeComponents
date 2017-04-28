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

#include <Live2DCubismCore.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

unsigned int csmGetSizeofFloatSink(const csmModel* model)
{
  int valueCount;


  valueCount  = csmGetParameterCount(model);
  valueCount += csmGetPartCount(model);
  valueCount += csmModelFloatTypeCount;


  return (unsigned int)(sizeof(csmFloatSink) + (sizeof(csmFloatSinkValue) * valueCount));
}

int csmGetIndexofFloatSinkValue(const csmFloatSink* sink, const char type, const csmHash id)
{
  int v;


  for (v = 0; v < sink->ValueCount; ++v)
  {
    if (sink->Values[v].Type != type || sink->Values[v].Id != id)
    {
      continue;
    }


    return v;
  }


  return -1;
}


void csmResetFloatSink(csmFloatSink* sink,
                       csmFloatSinkValue* values, const int valueCount)
{
  sink->ValueCount = valueCount;
  sink->Values = values;
}


csmFloatSink* csmInitializeFloatSinkInPlace(csmModel* model, void* address, const unsigned int size)
{
  csmFloatSinkValue* values;
  csmFloatSink* sink;
  int v, p;


  // 'Patch' pointers.
  sink = address;
  values = (csmFloatSinkValue*)(sink + 1);


  // Initialize parameter floats.
  for (v = 0, p = 0; p < csmGetParameterCount(model); ++p, ++v)
  {
    values[v].Type = csmParameterFloat;
    values[v].IsDirty = 0;
    values[v].Id = csmHashString(csmGetParameterIds(model)[p]);
    values[v].Value = csmGetParameterValues(model)[p];
  }


  // Initialize part opacity floats.
  for (p = 0; p < csmGetPartCount(model); ++p, ++v)
  {
    values[v].Type = csmPartOpacityFloat;
    values[v].IsDirty = 0;
    values[v].Id = csmHashString(csmGetPartIds(model)[p]);
    values[v].Value = csmGetPartOpacities(model)[p];
  }


  // Initialize model floats.
  for (p = 0; p < csmModelFloatTypeCount; ++p, ++v)
  {
    values[v].Type = csmModelFloat;
    values[v].IsDirty = 0;
    values[v].Id = p;
    values[v].Value = 1.0f;
  }


  // Finalize initialization.
  csmResetFloatSink(sink, values, v);


  return sink;
}


void csmFlushFloatSink(csmFloatSink *sink, csmModel *model, csmModelFloatHandler onModelFloat, void* userData)
{
  float* parameterValues, * partOpacities;
  int v, vShift;


  // Get addresses to write to.
  parameterValues = csmGetParameterValues(model);
  partOpacities = csmGetPartOpacities(model);


  // Flush parameter values. (We know they're sequential)...
  for (v = 0; sink->Values[v].Type == csmParameterFloat; ++v)
  {
    // Skip non-dirty values.
    if (!sink->Values[v].IsDirty)
    {
      continue;
    }


    parameterValues[v] = sink->Values[v].Value;


    sink->Values[v].IsDirty = 0;
  }


  // Flush part opacities. (Again, we know they're sequential)...
  for (vShift = -v; sink->Values[v].Type == csmPartOpacityFloat; ++v)
  {
    // Skip non-dirty values.
    if (!sink->Values[v].IsDirty)
    {
      continue;
    }


    partOpacities[v + vShift] = sink->Values[v].Value;


    sink->Values[v].IsDirty = 0;
  }


  // Let user handle model floats. (And, again, we know they're sequential)...
  if (onModelFloat)
  {
    for (; sink->Values[v].Type == csmModelFloat; ++v)
    {
      // Skip non-dirty values.
      if (!sink->Values[v].IsDirty)
      {
        continue;
      }


      onModelFloat(sink, sink->Values[v].Id, sink->Values[v].Value, userData);


      sink->Values[v].IsDirty = 0;
    }
  }
}
