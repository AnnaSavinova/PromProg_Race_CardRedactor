#pragma once
#include "CWindow.h"

extern IUIFramework* g_pFramework;  // Reference to the Ribbon framework.
extern CWindow* g_pApplication;  // Reference to the Application object

// Methods to facilitate the initialization and destruction of the Ribbon framework.
HRESULT InitRibbon( HWND hWnd );
void DestroyRibbon();