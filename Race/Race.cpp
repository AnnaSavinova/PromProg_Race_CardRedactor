#include "stdafx.h"
#include "CWindow.h"

CComModule _Module;
CWindow MainWindow;

int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	CoInitialize( NULL );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	MainWindow.RegisterClass();
	MainWindow.Create();
	MainWindow.Show( nCmdShow );
	MainWindow.StartNewGame();

	HACCEL table = ::LoadAccelerators( hInstance, MAKEINTRESOURCE( IDR_ACC1 ) );
	MSG msg;
	while ( ::GetMessage( &msg, NULL, 0, 0 ) > 0 ) {
		if ( !::IsDialogMessage( MainWindow.GetHandleDialog(), &msg ) && !::TranslateAccelerator( MainWindow.GetHandle(), table, &msg ) ) {
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}

	CoUninitialize();

	return (int)msg.wParam;
}
