/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */
 
 
 #include "Local.h"

 #include <stdlib.h>
 #include <stdio.h>


// ----- //
// TYPES //
// ----- //

/// OpenGL program abstraction layer.
typedef struct Program
{
  // Program handle.
  GLuint Handle;


  // Input locations.
  struct
  {
    /// Location of model-view-projection uniform.
    GLint Mvp;

    /// Location of opacity uniform.
    GLint Opacity;

    /// Location of mask texture.
    GLint MaskTexture;

    /// Location of diffuse texture.
    GLint DiffuseTexture;
  }
  Locations;
}
Program;


// ----------------- //
// SHADER CODE PARTS //
// ----------------- //

/// TODO  Document!
static const GLchar Gl33VertexShaderGlobals[] =
"#version 330\n"


"in vec2 VertexPosition;"
"in vec2 VertexUv;"


"out vec4 Color;"
"out vec2 MaskUv;"
"out vec2 DiffuseUv;";

/// TODO  Document!
static const GLchar Gles20VertexShaderGlobals[] =
"attribute vec2 VertexPosition;"
"attribute vec2 VertexUv;"


"varying vec4 Color;"
"varying vec2 MaskUv;"
"varying vec2 DiffuseUv;";

/// TODO  Document!
static const GLchar CommonVertexShaderGlobals[] =
"uniform mat4 Mvp;"
"uniform float Opacity;";

/// TODO  Document!
static const GLchar CommonVertexShaderMain[] =
"void main()"
"{"
"  vec4 position = Mvp * vec4(VertexPosition, 0.0, 1.0);"


"  vec4 screenPosition = position * 0.5;"


"  screenPosition.xy = screenPosition.xy + screenPosition.w;"
"  screenPosition.zw = position.zw;"


"  gl_Position = position;"


"  Color = vec4(1, 1, 1, Opacity);"
"  MaskUv = screenPosition.xy;"
"  DiffuseUv = VertexUv;"
"}";


/// TODO  Document!
static const GLchar Gl33FragmentShaderGlobals[] =
"#version 330\n"


"in vec4 Color;"
"in vec2 MaskUv;"
"in vec2 DiffuseUv;"


"out vec4 FragColor;";

/// TODO  Document!
static const GLchar Gles20FragmentShaderGlobals[] =
"varying vec4 Color;"
"varying vec2 MaskUv;"
"varying vec2 DiffuseUv;";

/// TODO  Document!
static const GLchar CommonFragmentShaderGlobals[] =
"uniform sampler2D MaskTexture;"
"uniform sampler2D DiffuseTexture;";

/// TODO  Document!
static const GLchar Gl33NonMaskedFragmentShaderMain[] =
"void main()"
"{"
"  vec4 fragColor = texture(DiffuseTexture, DiffuseUv) * Color;"


"  fragColor.rbg *= fragColor.a;"


"  FragColor = fragColor;"
"}";

/// TODO  Document!
static const GLchar Gles20NonMaskedFragmentShaderMain[] =
"void main()"
"{"
"  vec4 fragColor = texture(DiffuseTexture, DiffuseUv) * Color;"


"  fragColor.rbg *= fragColor.a;"


"  gl_FragColor = fragColor;"
"}";

/// TODO  Document!
static const GLchar Gl33MaskedFragmentShaderMain[] =
"void main()"
"{"
"  vec4 fragColor = texture(DiffuseTexture, DiffuseUv) * Color;"
"  float maskValue = texture(MaskTexture, MaskUv).a;"


"  fragColor.rbg *= fragColor.a;"


"  FragColor = fragColor * maskValue;"
"}";

/// TODO  Document!
static const GLchar Gles20MaskedFragmentShaderMain[] =
"void main()"
"{"
"  vec4 fragColor = texture(DiffuseTexture, DiffuseUv) * Color;"
"  float maskValue = texture(MaskTexture, MaskUv).a;"


"  fragColor.rbg *= fragColor.a;"


"  gl_FragColor = fragColor * maskValue;"
"}";


/// Location of vertex position.
static const GLint VertexPositionLocation = 0;

/// Location of vertex UV.
static const GLint VertexUvLocation = 1;


// --------- //
// FUNCTIONS //
// --------- //

/// Creates a program.
///
/// @param  vertexShaderStrings        Vertex shader code.
/// @param  vertexShaderStringCount    Number of strings making up vertex shader.
/// @param  fragmentShaderStrings      Fragment shader code.
/// @param  fragmentShaderStringCount  Number of strings making up fragment shader.
///
/// @return  Program.
static Program MakeProgram(const GLchar** vertexShaderStrings,
                           const GLsizei vertexShaderStringCount,
                           const GLchar** fragmentShaderStrings,
                           const GLsizei fragmentShaderStringCount)
{
	GLuint vertexShader, fragmentShader;
  Program program;


	// Make vertex shader.
	vertexShader = glCreateShader(GL_VERTEX_SHADER);


	glShaderSource(vertexShader, vertexShaderStringCount, vertexShaderStrings, 0);
	glCompileShader(vertexShader);


	// Make fragment shader.
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);


	glShaderSource(fragmentShader, fragmentShaderStringCount, fragmentShaderStrings, 0);
	glCompileShader(fragmentShader);


	// Create program and attach shaders.
	program.Handle = glCreateProgram();


	glAttachShader(program.Handle, vertexShader);
	glAttachShader(program.Handle, fragmentShader);


	// Bind vertex attribute locations.
	glBindAttribLocation(program.Handle, VertexPositionLocation, "VertexPosition");
	glBindAttribLocation(program.Handle, VertexUvLocation, "VertexUv");


	// Link program.
	glLinkProgram(program.Handle);


	// Clean up shaders.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


  // Initialize locations.
  program.Locations.Mvp = glGetUniformLocation(program.Handle, "Mvp");
  program.Locations.Opacity = glGetUniformLocation(program.Handle, "Opacity");
  program.Locations.MaskTexture = glGetUniformLocation(program.Handle, "MaskTexture");
  program.Locations.DiffuseTexture = glGetUniformLocation(program.Handle, "DiffuseTexture");


	return program;
}

/// Releases OpenGL resources of a program.
///
/// @param  program  Program to release.
static void ReleaseProgram(Program* program)
{
  glDeleteProgram(program->Handle);
}


// --------- //
// VARIABLES //
// --------- //

/// Vertex shader code.
static const GLchar* CommonVertexShaderCode[] =
{
#if _CSM_USE_GL33
  Gl33VertexShaderGlobals,
#elif _CSM_USE_GLES20
  Gles20VertexShaderGlobals,
#endif
  CommonVertexShaderGlobals,
  CommonVertexShaderMain
};

/// Non-masked fragment shader code.
static const GLchar* NonMaskedFragmentShaderCode[] =
{
#if _CSM_USE_GL33
  Gl33FragmentShaderGlobals,
#elif _CSM_USE_GLES20
  Gles20FragmentShaderGlobals,
#endif
  CommonFragmentShaderGlobals,
#if _CSM_USE_GL33
  Gl33NonMaskedFragmentShaderMain,
#elif _CSM_USE_GLES20
  Gles20NonMaskedFragmentShaderMain,
#endif
};

/// Non-masked fragment shader code.
static const GLchar* MaskedFragmentShaderCode[] =
{
#if _CSM_USE_GL33
  Gl33FragmentShaderGlobals,
#elif _CSM_USE_GLES20
  Gles20FragmentShaderGlobals,
#endif
  CommonFragmentShaderGlobals,
#if _CSM_USE_GL33
  Gl33MaskedFragmentShaderMain,
#elif _CSM_USE_GLES20
  Gles20MaskedFragmentShaderMain,
#endif
};


/// Programs reference counter.
static int RetainPrograms = 0;

/// Programs.
static Program Programs[2] = {0};

/// Currently active program.
static int ActiveProgram = 0;


// -------------- //
// IMPLEMENTATION //
// -------------- //

void RequireGlPrograms()
{
  // Initialize programs if necessary.
  if (!RetainPrograms)
  {
    LoadGl();


    Programs[0] = MakeProgram(CommonVertexShaderCode,
                              (GLsizei)(sizeof(CommonVertexShaderCode) / sizeof(const char*)),
                              NonMaskedFragmentShaderCode,
                              (GLsizei)(sizeof(NonMaskedFragmentShaderCode) / sizeof(const char*)));

    Programs[1] = MakeProgram(CommonVertexShaderCode,
                              (GLsizei)(sizeof(CommonVertexShaderCode) / sizeof(const char*)),
                              MaskedFragmentShaderCode,
                              (GLsizei)(sizeof(MaskedFragmentShaderCode) / sizeof(const char*)));
  }


  ++RetainPrograms;
}

void UnrequireGlPrograms()
{
  int p;


  --RetainPrograms;


  // Release programs if appropriate.
  if (!RetainPrograms)
  {
    for (p = 0; p < (int)(sizeof(Programs) / sizeof(Programs[0])); ++p)
    {
      ReleaseProgram(&Programs[p]);
    }
  }
}


void ActivateGlProgram(GlProgram program)
{
  ActiveProgram = program % 2;


  glUseProgram(Programs[ActiveProgram].Handle);
}


GLint GetGlVertexPositionLocation()
{
  return VertexPositionLocation;
}

GLint GetGlVertexUvLocation()
{
  return VertexUvLocation;
}


void SetGlMvp(const GLfloat* value)
{
  glUniformMatrix4fv(Programs[ActiveProgram].Locations.Mvp, 1, GL_FALSE, value);
}

void SetGlOpacity(const float value)
{
  glUniform1f(Programs[ActiveProgram].Locations.Opacity, value);
}

void SetGlMaskTexture(const GLuint value)
{
	glUniform1i(Programs[ActiveProgram].Locations.MaskTexture, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, value);
}

void SetGlDiffuseTexture(const GLuint value)
{
	glUniform1i(Programs[ActiveProgram].Locations.DiffuseTexture, 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, value);
}
