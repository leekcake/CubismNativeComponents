/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "App.h"


// -------- //
// REQUIRES //
// -------- //

#include "Gl.h"
#include "ISample.h"

#include <SDL.h>


// --------- //
// FUNCTIONS //
// --------- //

/// Initializes matrix.
///
/// @param  vp  Matrix to initialize.
/// @param  windowWidth  Window width in pixels.
/// @param  windowHeight  Window height in pixels.
/// @param  safeFrame     Minimum units that should be visible.
static void InitializeVp(GLfloat* vp, float windowWidth, float windowHeight, float safeFrame)
{
	float aspect, xScale, yScale;


	// Compute scale.
  aspect = windowWidth / windowHeight;


	if (aspect > 1.0f)
	{
		yScale = (1.0f / safeFrame) * 2.0f;
  	xScale = yScale * (1.0f / aspect);
	}
	else
	{
		aspect = windowHeight / windowWidth;


		xScale = (1.0f / safeFrame) * 2.0f;
  	yScale = xScale * (1.0f / aspect);
	}


  // Set matrix.
  vp[0]  = xScale; vp[1]  = 0.0f;   vp[2]  = 0.0f; vp[3]  = 0.0f;
  vp[4]  = 0.0f;   vp[5]  = yScale; vp[6]  = 0.0f; vp[7]  = 0.0f;
  vp[8]  = 0.0f;   vp[9]  = 0.0f;   vp[10] = 1.0f; vp[11] = 0.0f;
  vp[12] = 0.0f;   vp[13] = 0.0f;   vp[14] = 0.0f; vp[15] = 1.0f;
}

// --------- //
// CONSTANTS //
// --------- //

/// App title.
static const char* AppTitle = "Cubism Sample";

enum
{
	/// Window width in pixels.
  WindowWidth  = 1280,

	/// Window height in pixels.
	WindowHeight = 720
};

/// Dimenson of safe frame in units.
static const float SafeFrame = 2.0f;

/// View projection matrix.
static GLfloat Vp[16];


// -------------- //
// IMPLEMENTATION //
// -------------- //

const GLfloat* GetViewProjectionMatrix()
{
	return Vp;
}


// ----------- //
// ENTRY POINT //
// ----------- //

// Called by SDL2.
//
// @param   argumentCount  [Unused] Number of arguments passed.
// @param   arguments      [Unused] Arguments.
//
// @return  Always returns '0'.
int main(int argumentCount, char *arguments[])
{
  int glDidLoad, didAppStart, windowWidth, windowHeight, exitLoop;
	Uint64 counter, lastCounter;
	SDL_GLContext glContext;
	SDL_Window* window;
	SDL_Event event;
	float deltaTime;


  // Initialize SDL2 video.
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		return 0;
	}


	// Make sure to get a double-buffered OpenGL 3.3 debug context.
  SDL_GL_LoadLibrary(0);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, SDL_TRUE);
#if _CSM_USE_GLES20
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif _CSM_USE_GL33
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);


	// Create window.
	window = SDL_CreateWindow(AppTitle,
	                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                          WindowWidth, WindowHeight,
		                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);


	// Exit on error.
	if (!window)
	{
		goto CleanUp;
	}


	// Create OpenGL context.
	glContext = SDL_GL_CreateContext(window);


	// Exit on error.
	if (!glContext)
	{
		goto CleanUp;
	}


	// Load OpenGL functions.
#if _CSM_INCLUDE_GLAD
  glDidLoad = gladLoadGL();


	// Exit on error.
  if (!glDidLoad)
  {
  	goto CleanUp;
  }
#endif


  // Let sample start.
  OnDidStart();


	// Set view projection.
  SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	InitializeVp(Vp, windowWidth, windowHeight, SafeFrame);


  // Loop.
	SDL_GL_SetSwapInterval(1);


	lastCounter = SDL_GetPerformanceCounter();
	exitLoop = 0;
	

	while (!exitLoop)
	{
		// Poll events.
		while (SDL_PollEvent(&event) != 0)
		{
			// Handle exit request.
			if (event.type == SDL_QUIT)
			{
				exitLoop = SDL_TRUE;
			}
		}


    // Clear frame buffer.
		glClear(GL_COLOR_BUFFER_BIT);


    // Tick counter to compute delta time.
		counter = SDL_GetPerformanceCounter();
		deltaTime = (((counter - lastCounter) * 1000) / (float)SDL_GetPerformanceFrequency()) * 0.001f;
		lastCounter = counter;


		// Trigger tick event.
    OnTick(deltaTime);


		// Swap buffers.
		SDL_GL_SwapWindow(window);
	}


  // Be tidy.
  CleanUp:


  // Trigger quit event.
  OnWillQuit();


  // Release SDL.
  if (glContext)
	{
		SDL_GL_DeleteContext(glContext);
	}


	if (window)
	{
		SDL_DestroyWindow(window);
	}


	SDL_Quit();


  return 0;
}
