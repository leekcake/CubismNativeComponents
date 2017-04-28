/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include <SampleModel.h>


// -------- //
// REQUIRES //
// -------- //

#include "Helper/Allocation.h"
#include "Helper/App.h"
#include "Helper/Gl.h"

#include <SDL_rwops.h>
#include <stb_image.h>


// --------- //
// FUNCTIONS //
// --------- //

/// Loads a moc from disk.
///
/// @param  path  Path to moc.
///
/// @return Deserialized moc.
csmMoc* LoadMoc(const char* path)
{
  unsigned int size;
  SDL_RWops* file;
  void* address;
  csmMoc* moc;


  // Open file.
  file = SDL_RWFromFile(path, "rb");


  // Get moc size in bytes.
  size = SDL_RWsize(file);


  // Allocate aligned memory for moc.
  address = AllocateAligned(size, csmAlignofMoc);


  // Read file into memory and close it afterwards.
  SDL_RWread(file, address, sizeof(char), size);
  SDL_RWclose(file);


  // Revive moc and return it.
  moc = csmReviveMocInPlace(address, size);


  return moc;
}

/// Instantiates a model.
///
/// @param  moc  Moc to instantiate.
///
/// @return Instance.
csmModel* MakeModel(const csmMoc* moc)
{
  unsigned int size;
  csmModel* model;
  void* address;


  // Get size of model.
  size = csmGetSizeofModel(moc);


  // Allocate aligned memory for model.
  address = AllocateAligned(size, csmAlignofModel);


  // Instantiate model and return it.
  model = csmInitializeModelInPlace(moc, address, size);


  return model;
}

/// Loads a PNG texture.
///
/// @param  path  Path to PNG.
///
/// @return  OpenGL texture handle.
GLuint LoadPng(const char* path)
{
	int width, height, channels;
	stbi_uc* png, * address;
	unsigned int size;
  SDL_RWops* file;
	GLuint texture;


  // Open file.
  file = SDL_RWFromFile(path, "rb");


  // Get moc size in bytes.
  size = SDL_RWsize(file);


  // Allocate memory for PNG and read it into memory.
  address = Allocate(size);


  SDL_RWread(file, address, sizeof(char), size);


	// Flip for making OpenGL happy.
	stbi_set_flip_vertically_on_load(1);


	// Load png.
	png = stbi_load_from_memory(address,
		(int)size,
		&width,
		&height,
		&channels,
		STBI_rgb_alpha);


	// Create OpenGL texture.
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Unbind texture and release PNG..
	glBindTexture(GL_TEXTURE_2D, 0);


  // Clean up.
	stbi_image_free(png);
	Deallocate(address);
  SDL_RWclose(file);


	return texture;
}

/// Creates an OpenGL renderer of a model.
///
/// @param  model  Model to create renderer of.
///
/// @return Renderer.
csmGlRenderer* MakeGlRenderer(csmModel* model)
{
  csmGlRenderer* renderer;
  unsigned int size;
  void* address;


  // Get size of renderer.
  size = csmGetSizeofGlRenderer(model);


  // Allocate memory for renderer.
  address = Allocate(size);


  // Create renderer and return it.
  renderer = csmMakeGlRendererInPlace(model, address, size);


  return renderer;
}

/// Loads an animation from disk.
///
/// @param  path  Path to serialized motion.
///
/// @return  Deserialized motion.
csmAnimation* LoadAnimation(const char* path)
{
  csmAnimation* animation;
  unsigned int size;
  char* motionJson;
  SDL_RWops* file;
  void* address;


  // Read in motion file.
  file = SDL_RWFromFile(path, "rb");
  motionJson = Allocate((unsigned int)SDL_RWsize(file));


  SDL_RWread(file, motionJson, sizeof(char), (unsigned int)SDL_RWsize(file));
  SDL_RWclose(file);


  // Get deserialized size of motion.
  size = csmGetDeserializedSizeofAnimation(motionJson);


  // Allocate memory for animation.
  address = Allocate(size);


  // Deserialize motion.
  animation = csmDeserializeAnimationInPlace(motionJson, address, size);


  // Clean up and return animation.
  Deallocate(motionJson);


  return animation;
}

/// Creates a float sink.
///
/// @param  model  Model to create float sink for.
///
/// @return Float sink.
csmFloatSink* MakeFloatSink(csmModel* model)
{
  csmFloatSink* sink;
  unsigned int size;
  void* address;


  // Get size of sink.
  size = csmGetSizeofFloatSink(model);


  // Allocate memory for sink.
  address = Allocate(size);


  // Initialize sink and return it.
  sink = csmInitializeFloatSinkInPlace(model, address, size);


  return sink;
}


// -------------- //
// IMPLEMENTATION //
// -------------- //

SampleModel LoadModel(const char* mocPath, const char* pngPath, const char** motionJsonPaths)
{
  int a;

  SampleModel model = {0};


  model.Moc = LoadMoc(mocPath);
  model.Model = MakeModel(model.Moc);
  model.Renderer = MakeGlRenderer(model.Model);


  // Load animations if feasible.
  if (motionJsonPaths)
  {
    for (a = 0; a < SampleModelAnimationCapacity; ++a)
    {
      model.Animations[a] = LoadAnimation(motionJsonPaths[a]);
    }
  }


  model.Sink = MakeFloatSink(model.Model);
  model.Mvp = GetViewProjectionMatrix();
  model.Texture = LoadPng(pngPath);


  return model;  
}

void ReleaseModel(SampleModel* model)
{
  int a;


  // Release resources.
  glDeleteTextures(1, &model->Texture);


  csmReleaseGlRenderer(model->Renderer);


  // Free memory.
  Deallocate(model->Sink);


  for (a = 0; a < SampleModelAnimationCapacity; ++a)
  {
    if (!model->Animations[a])
    {
      continue;
    }


    Deallocate(model->Animations[a]);
  }


  Deallocate(model->Renderer);
  DeallocateAligned(model->Model);
  DeallocateAligned(model->Moc);
}
