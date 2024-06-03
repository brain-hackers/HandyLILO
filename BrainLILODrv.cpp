/*
 * BrainLILODrv
 * U-Boot loader for electric dictionary.
 *
 * Copyright (C) 2019 C. Shirasaka <holly_programmer@outlook.com>
 * based on
 ** ResetKitHelper
 ** Soft/hard reset the electronic dictionary.
 **
 ** Copyright (C) 2012 T. Kawada <tcppjp [ at ] gmail.com>
 *
 * This file is licensed in MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <pm.h>
#include <stdint.h>
#include <windows.h>

#define BRAINLILODRV_API __declspec(dllexport)

#include "BrainLILODrv.h"

static void boothelper()
{
    asm volatile("mrc	p15,0,r10,c1,c0,0\n" // read ctrl regs
                 "bic	r10, r10, #5\n"      // disable MMU/DCache
                 "mcr	p15,0,r10,c1,c0,0\n" // write ctrl regs
                 "mov   r1, #0x80000000\n"
                 "orr   r1,r1,#0x800000\n"
                 "bx    r1\n");
}

static void EDNA2_installPhysicalInvoker()
{
    void *ptr = (void *)0xA10B6000;
    wchar_t buf[256];
    swprintf(buf, L"BrainLILO: copying boot helper to 0x%08x from 0x%08x\n", (int)(ptr), (int)(&boothelper));
    OutputDebugString(buf);
    memcpy(ptr, (const void *)&boothelper, 64);
}

static bool doLinux()
{

    TCHAR bootloaderFileName[128] = TEXT("\\Storage Card\\loader\\u-boot.bin");
    HANDLE hFile;
    wchar_t buf[256];
    DWORD wReadSize, FileSize;

    OutputDebugString(L"BrainLILO: Opening Bootloader file...");
    hFile = CreateFile(bootloaderFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        OutputDebugString(L"Cant open bootloader");
        return false;
    }
    swprintf(buf, L"BrainLILO: Bootloader file handle 0x%08x\n", (int)(hFile));
    OutputDebugString(buf);

    FileSize = GetFileSize(hFile, NULL);
    swprintf(buf, L"BrainLILO: Bootloader file size %d Byte\n", FileSize);
    OutputDebugString(buf);

    swprintf(buf, L"BrainLILO: Loading bootloader to 0x%08x...\n", 0xa0800000);
    OutputDebugString(buf);
    if (!ReadFile(hFile, (void *)0xa0800000, FileSize, &wReadSize, NULL))
    {
        OutputDebugString(L"Cant read bootloader");
        return false;
    }
    OutputDebugString(L"BrainLILO: Bootloader copied! Closing file handle...");
    CloseHandle(hFile);

    OutputDebugString(L"BrainLILO: Kernel Entry Point override with branch instruction to U-Boot...");
    EDNA2_installPhysicalInvoker();

    OutputDebugString(L"BrainLILO: Warm reset initiate...");
    SetSystemPowerState(NULL, POWER_STATE_RESET, 0);
    return true;
}

extern "C" BRAINLILODRV_API BOOL LIN_IOControl(DWORD handle, DWORD dwIoControlCode, DWORD *pInBuf, DWORD nInBufSize,
                                               DWORD *pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned)
{
    SetLastError(0);

    switch (dwIoControlCode)
    {
    case IOCTL_LIN_DO_LINUX:
        doLinux();
        return TRUE;
    }
    return FALSE;
}

extern "C" BRAINLILODRV_API BOOL LIN_Read(DWORD handle, LPVOID pBuffer, DWORD dwNumBytes)
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

extern "C" BRAINLILODRV_API BOOL LIN_Write(DWORD handle, LPVOID pBuffer, DWORD dwNumBytes)
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

extern "C" BRAINLILODRV_API DWORD LIN_Seek(DWORD handle, long lDistance, DWORD dwMoveMethod)
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

extern "C" BRAINLILODRV_API void LIN_PowerUp(void)
{
    OutputDebugString(L"BrainLILO: resuming.");
}

extern "C" BRAINLILODRV_API void LIN_PowerDown(void)
{
}

extern "C" BRAINLILODRV_API DWORD LIN_Init(LPCTSTR pContext, DWORD dwBusContext)
{

    void *ctx;
    ctx = (void *)LocalAlloc(LPTR, sizeof(4));

    return (DWORD)ctx;
}

extern "C" BRAINLILODRV_API DWORD LIN_Open(DWORD dwData, DWORD dwAccess, DWORD dwShareMode)
{

    void *hnd = (void *)LocalAlloc(LPTR, 4);
    return (DWORD)hnd;
}

extern "C" BRAINLILODRV_API BOOL LIN_Close(DWORD handle)
{
    LocalFree((void *)handle);

    return TRUE;
}

extern "C" BRAINLILODRV_API BOOL LIN_Deinit(DWORD dwContext)
{

    LocalFree((void *)dwContext);
    return TRUE;
}

extern "C" BOOL APIENTRY DllMainCRTStartup(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
