#include "header.h"
#include <iostream>

//Gets address of function from OpenGL library
void GetOpenGLFunctions()
{
	HMODULE Moduleandle = GetModuleHandle(L"opengl32.dll");
	if (Moduleandle != nullptr)
	{
		glDrawElementsAddress = (DWORD)GetProcAddress(Moduleandle, "glDrawElements");
	}
	else
	{
		MessageBox(0, L"Module handle not found", L"Message", MB_OK);
	}
}

//Hooked function that will be executed instead of original function in the process
void WINAPI hkglDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
	//Restores bytes of original function
	BYTE* codeDest = (BYTE*)oglDrawElements;
	codeDest[0] = codeFragment_glDrawElements[0];
	*((DWORD*)(codeDest + 1)) = *((DWORD*)(codeFragment_glDrawElements + 1));

	if (isEnabled)
	{
		if (count > 260) 
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	oglDrawElements(mode, count, type, indices);

	//Restores bytes of hooked function so process of the game will execute hooked function instead of original function 
	codeDest[0] = jump_glDrawElements[0];
	*((DWORD*)(codeDest + 1)) = *((DWORD*)(jump_glDrawElements + 1));
}

//Performs code injection
void HookOpenGLFunctions()
{

	oglDrawElements = (tglDrawElements)glDrawElementsAddress; //Gets address of original function

	jump_glDrawElements[0] = 0xE9;
	DWORD addr_glBegin = (DWORD)hkglDrawElements - (DWORD)oglDrawElements - 5; //Gets address of injection (where to jump)
	memcpy(jump_glDrawElements + 1, &addr_glBegin, sizeof(DWORD)); //Adds address after 'jmp' opcode to get assembler instruction jmp [address]
	memcpy(codeFragment_glDrawElements, oglDrawElements, 5); //Saves address of original function
	VirtualProtect(oglDrawElements, 8, PAGE_EXECUTE_READWRITE, &savedProtection_glDrawElements);
	memcpy(oglDrawElements, jump_glDrawElements, 5); //Replaces address of original function with assembler instruction jmp [address] (jump to injection where hook is)
}

DWORD WINAPI TF(void* lParam)
{
	GetOpenGLFunctions();
	HookOpenGLFunctions();
	return 0;
}

DWORD WINAPI KeyboardHook(void* lParam)
{
	while (1)
	{
		if (GetAsyncKeyState(VK_NUMPAD1))
		{
			isEnabled = !isEnabled;
		}
		Sleep(100);
	}
	return 0;
}

int WINAPI DllMain(HINSTANCE hInst, DWORD ul_reason_for_call, void* lpReserver)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, &TF, 0, 0, 0);
		CreateThread(0, 0, &KeyboardHook, 0, 0, 0);
	}
	return 1;
}