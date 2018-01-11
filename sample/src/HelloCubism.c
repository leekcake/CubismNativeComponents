/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "Local.h"


// -------- //
// REQUIRES //
// -------- //

#include <Live2DCubismCore.h>
#include <Live2DCubismFramework.h>
#include <Live2DCubismGlRendering.h>
#include <SDL.h>


// -------------- //
// IMPLEMENTATION //
// -------------- //

static struct
{
  csmMoc* Moc;

  csmModel* Model;

  csmModelHashTable* Table;

  csmAnimation* Animation;

  csmUserData* UserData;

  csmAnimationUserDataCallbackState AnimationUserDataCallback;

  csmAnimationState AnimationState;

  csmPhysicsRig* Physics;

  csmGlRenderer* Renderer;

  GLuint* Textures;
}
Sample;

static const char* TexturePathes[] =
{
  "Mark/Mark.png",
};

static int GetTextureCount()
{
  return sizeof(TexturePathes) / sizeof(const char*);
}

const char* GetSampleTitle()
{
  return "Hello Cubism";
}

float GetSampleSize()
{
  return 2.0f;
}

void Callback(float time, const char* value)
{
  SDL_Log("%f %s\r\n", time, value);
}

void OnDidStart()
{
  void* mocMemory, * modelMemory, * tableMemory, * motionJsonMemory, * animationMemory, * rendererMemory;
  unsigned int mocSize, modelSize, tableSize, animationSize, rendererSize, textureArraySize;

  void* userDataJsonMemory, * userDataMemory, * physicsJsonMemory, * physicsMemory;
  unsigned int userDataSize, physicsSize;

  const char* userDataTag;

  csmUserDataSink userDataSink;
  csmAnimationUserDataSink animationSink;

  int textureCount;

  int i;


  // Load and revive moc.
  mocMemory = ReadBlobAligned("Mark/Mark.moc3", csmAlignofMoc, &mocSize);


  Sample.Moc = csmReviveMocInPlace(mocMemory, mocSize);


  // Instantiate moc.
  modelSize = csmGetSizeofModel(Sample.Moc);
  modelMemory = AllocateAligned(modelSize, csmAlignofModel);


  Sample.Model = csmInitializeModelInPlace(Sample.Moc, modelMemory, modelSize);


  // Create hash table.
  tableSize = csmGetSizeofModelHashTable(Sample.Model);
  tableMemory = Allocate(tableSize);


  Sample.Table = csmInitializeModelHashTableInPlace(Sample.Model, tableMemory, tableSize);


  // Load user data.
  userDataJsonMemory = ReadBlob("Mark/Mark.userdata3.json", 0);


  userDataSize = csmGetDeserializedSizeofUserData(userDataJsonMemory);
  userDataMemory = Allocate(userDataSize);
  
    
  Sample.UserData = csmDeserializeUserDataInPlace(userDataJsonMemory, userDataMemory, userDataSize);

  // Try to getting user data.
  for (i = 0; i < csmGetUserDataCount(Sample.UserData); ++i)
  {
    csmGetUserData(Sample.UserData, i, &userDataSink);

    SDL_Log("%s\r\n", userDataSink.Value);
  }

  Deallocate(userDataJsonMemory);


  // Load physics.
  physicsJsonMemory = ReadBlob("Mark/Mark.physics3.json", 0);


  physicsSize = csmGetDeserializedSizeofPhysics(physicsJsonMemory);
  physicsMemory = Allocate(physicsSize);


  Sample.Physics = csmDeserializePhysicsInPlace(physicsJsonMemory, physicsMemory, physicsSize);

  Deallocate(physicsJsonMemory);


  // Load animation.
  motionJsonMemory = ReadBlob("Mark/Mark.motion3.json", 0);


  animationSize = csmGetDeserializedSizeofAnimation(motionJsonMemory);
  animationMemory = Allocate(animationSize);


  Sample.Animation = csmDeserializeAnimationInPlace(motionJsonMemory, animationMemory, animationSize);

  Deallocate(motionJsonMemory);


  // Try to getting animation user data.
  for (i = 0; i < csmGetAnimationUserDataCount(Sample.Animation); ++i)
  {
      csmGetAnimationUserData(Sample.Animation, i, &animationSink);

      SDL_Log("%s\r\n", animationSink.Value);
  }


  // Initializes animation user data callback.
  csmInitializeAnimationUserDataCallback(&Sample.AnimationUserDataCallback, Callback);


  // Initialize animation state by resetting it.
  csmResetAnimationState(&Sample.AnimationState);


  // Create renderer.
  rendererSize = csmGetSizeofGlRenderer(Sample.Model);
  rendererMemory = Allocate(rendererSize);


  Sample.Renderer = csmMakeGlRendererInPlace(Sample.Model, rendererMemory, rendererSize);


  // Load texture.
  textureCount = GetTextureCount();
  textureArraySize = csmGetSizeofTextureArray(textureCount);


  Sample.Textures = Allocate(textureArraySize);

  for (i = 0; i < textureCount; ++i)
  {
    Sample.Textures[i] = LoadTextureFromPng(TexturePathes[i]);
  }
}


void OnTick(float deltaTime)
{
  float animationBlendWeight;
  csmPhysicsOptions phyicsOptions;


  phyicsOptions.Wind.X = 0.0f;
  phyicsOptions.Wind.Y = 0.0f;

  phyicsOptions.Gravity.X =  0.0f;
  phyicsOptions.Gravity.Y = -1.0f;


  animationBlendWeight = 1.0f;


  // Update animation.
  csmUpdateAnimationState(&Sample.AnimationState, deltaTime);
  csmEvaluateAnimationFAST(Sample.Animation,
                           &Sample.AnimationState,
                           csmOverrideFloatBlendFunction,
                           animationBlendWeight,
                           Sample.Model,
                           Sample.Table,
                           0,
                           0);
  
  // Update animtion userdata callback.
  csmUpdateAnimationUserDataCallbackUpdate(&Sample.AnimationUserDataCallback, &Sample.AnimationState, Sample.Animation);
  

  // Update physics.
  csmPhysicsEvaluate(Sample.Model, Sample.Physics, &phyicsOptions, deltaTime);


  // Update model.
  csmUpdateModel(Sample.Model);


  // Update renderer.
  csmUpdateGlRenderer(Sample.Renderer);


  // Reset dynamic drawable flags.
  csmResetDrawableDynamicFlags(Sample.Model);


  // Draw.
  csmGlDraw(Sample.Renderer, GetViewProjectionMatrix(), Sample.Textures);
}


void OnWillQuit()
{
  int i;


  // Release resources.  
  for (i = 0; i < GetTextureCount(); ++i)
  {
      ReleaseTexture(Sample.Textures[i]);
  }


  csmReleaseGlRenderer(Sample.Renderer);


  // Free memory.
  Deallocate(Sample.Renderer);
  Deallocate(Sample.UserData);
  Deallocate(Sample.Physics);
  Deallocate(Sample.Animation);
  Deallocate(Sample.Table);
  DeallocateAligned(Sample.Model);
  DeallocateAligned(Sample.Moc);
}
