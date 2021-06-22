#ifndef HEADER_H
#define HEADER_H

#include <Windows.h>
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

typedef void(WINAPI* tglDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
tglDrawElements  oglDrawElements;

BYTE codeFragment_glDrawElements[5] = { 0, 0, 0, 0, 0 };
BYTE jump_glDrawElements[5] = { 0, 0, 0, 0, 0 };
DWORD savedProtection_glDrawElements = 0;

DWORD glDrawElementsAddress = 0;

bool isEnabled = false;

#endif