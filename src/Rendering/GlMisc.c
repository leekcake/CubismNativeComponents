#include <Live2DCubismGlRenderingINTERNAL.h>

char csmIsSupportGL30() {
	static signed char _isSupportGL30 = -1;
	if (_isSupportGL30 != -1)
	{
		return _isSupportGL30;
	}
	int version = -1;
	glGetIntegerv(GL_MAJOR_VERSION, &version);
	if (version >= 3) {
#ifdef _CSM_COMPONENTS_ANDROID
		gl3stubInit();
#endif
		csmGetLogFunction()("Device look like support OpenGL (ES) 3");
		_isSupportGL30 = 1;
	}
	else
	{
		csmGetLogFunction()("Device look like didn't support OpenGL (ES) 3 :(");
		_isSupportGL30 = 0;
	}

	return _isSupportGL30;
}