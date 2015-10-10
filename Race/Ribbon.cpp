#include "stdafx.h"
#include <initguid.h>
#include "toolbar.h"
#include "CWindow.h"

IUIFramework* g_pFramework = NULL;

HRESULT InitRibbon( HWND hWindowFrame )
{
	::CoCreateInstance( CLSID_UIRibbonFramework, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &g_pFramework ) );

	CComObject<CWindow> Application;
	CComObject<CWindow>* pApplication = &Application;
	CComObject<CWindow>::CreateInstance( &pApplication );

	g_pFramework->Initialize( hWindowFrame, pApplication );
	g_pFramework->LoadUI( GetModuleHandle( NULL ), L"APPLICATION_RIBBON" );

	return 0;
}

void DestroyRibbon()
{
	if ( g_pFramework ) {
		g_pFramework->Destroy();
		g_pFramework->Release();
		g_pFramework = NULL;
	}
}
