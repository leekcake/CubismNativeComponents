/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */
 

#include "Local.h"

#include <stdio.h>


// ----- //
// TYPES //
// ----- //


// OpenGL mask buffer abstraction layer.
typedef struct Maskbuffer
{
  /// Mask buffer.
	GLuint Handle;

	/// Mask texture attached to mask buffer.
	GLuint Texture;

  /// Size of buffer in pixels.
  GLint Size;

	/// Unused depth renderbuffer attached to mask buffer.
	GLuint DepthAttachment;
}
Maskbuffer;


// --------- //
// FUNCTIONS //
// --------- //

/// TODO  Document.
static Maskbuffer MakeMaskbuffer(const GLint size)
{
  GLint userFramebuffer;
  Maskbuffer buffer;


  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &userFramebuffer);


	// Create texture.
	glGenTextures(1, &buffer.Texture);


	glBindTexture(GL_TEXTURE_2D, buffer.Texture);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glBindTexture(GL_TEXTURE_2D, 0);
	

  // Create (dummy) depth buffer.
	glGenRenderbuffers(1, &buffer.DepthAttachment);


	glBindRenderbuffer(GL_RENDERBUFFER, buffer.DepthAttachment);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size, size);


	glBindRenderbuffer(GL_RENDERBUFFER, 0);


  // Create framebuufer.
	glGenFramebuffers(1, &buffer.Handle);


	glBindFramebuffer(GL_FRAMEBUFFER, buffer.Handle);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.Texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer.DepthAttachment); 


 	glBindFramebuffer(GL_FRAMEBUFFER, userFramebuffer);


  // Store size.
  buffer.Size = size;


  return buffer;
}

/// TODO  Document.
static void ReleaseMaskbuffer(Maskbuffer* buffer)
{
  glDeleteFramebuffers(1, &buffer->Handle);
	glDeleteRenderbuffers(1, &buffer->DepthAttachment);
	glDeleteTextures(1, &buffer->Texture);
}


// --------- //
// VARIABLES //
// --------- //

static int RetainSingleton = 0;

static Maskbuffer Singleton = {0};


static GLint UserViewport[4];

static GLint UserFramebuffer;


// -------------- //
// IMPLEMENTATION //
// -------------- //

void RequireGlMaskbuffer()
{
  // Initialize singleton.
  if (!RetainSingleton)
  {
    Singleton = MakeMaskbuffer(512);
  }


  ++RetainSingleton;
}

void UnrequireGlMaskbuffer()
{
  --RetainSingleton;


  // Release singleton if appropriate.
  if (!RetainSingleton)
  {
    ReleaseMaskbuffer(&Singleton);
  }
}


void ActivateGlMaskbuffer()
{
  GLfloat userClearColor[4];


  // Store user viewport and framebuffer.
  glGetIntegerv(GL_VIEWPORT, UserViewport);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &UserFramebuffer);


  // TODO  Resize mask buffer to match user viewport.


  // Switch buffer and viewport.
  glBindFramebuffer(GL_FRAMEBUFFER, Singleton.Handle);
	glViewport(0, 0, Singleton.Size, Singleton.Size);


  // Wipe buffer.
  glGetFloatv(GL_COLOR_CLEAR_VALUE, userClearColor);


  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(userClearColor[0], userClearColor[1], userClearColor[1], userClearColor[2]);
}

GLuint DeactivateGlMaskbuffer()
{
  // Restore framebuffer and viewport.
  glBindFramebuffer(GL_FRAMEBUFFER, UserFramebuffer);
  glViewport(UserViewport[0], UserViewport[1], UserViewport[2], UserViewport[3]);


  // Return texture drawn on.
  return Singleton.Texture;
}
