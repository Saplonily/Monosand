#pragma once
#ifndef H_PCH
#define H_PCH

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>
#include "glad/glad.h"

using byte = unsigned char;
#define GL_CHECK_ERROR {\
GLenum err = glGetError();\
if (err != 0)\
printf("ERROR(line%d): %d\n", __LINE__, err);\
}\

#endif