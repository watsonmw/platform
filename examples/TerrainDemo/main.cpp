#include "stdafx.h"
#include "terraindemo.h"

#include "platform/log.h"

int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
                     LPSTR lpszCmdLine, int nCmdShow)
{
    Platform::Log::print(L"(Terrain Demo by Mark Watson - (c) 2000");

    TerrainDemo demo(hCurrentInst, L"");
    return demo.run() ? 0 : -1;
}
