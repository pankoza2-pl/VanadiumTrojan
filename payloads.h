#pragma once

#include <Windows.h>
#include <cmath>
#include "timer.h"
#include "virtual_screen.h"
#include "def.h"

#pragma  comment(lib,"WinMM.lib")
#pragma  comment(lib,"Msimg32.lib")

extern void SeedXorshift32(DWORD dwSeed);

//extern void ExecutePayload(TROJAN_PAYLOAD payload, int nTime);

extern void ExecuteShader(TROJAN_SHADER shader, int nTime);


//extern void Payload1(int t, HDC hdcScreen);


extern void Shader1(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader2(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader3(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader4(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader5(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader6(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader7(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader8(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader9(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader10(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader11(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader12(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader13(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader14(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader15(int t, int w, int h, PRGBQUAD prgbScreen);
extern void Shader16(int t, int w, int h, PRGBQUAD prgbScreen);
