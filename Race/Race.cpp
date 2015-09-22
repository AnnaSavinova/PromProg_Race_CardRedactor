#include "stdafx.h"


int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    CWindow win;
    win.RegisterClass();
    win.Create();
    win.Show( nCmdShow );
    win.StartNewGame();

    HACCEL table = ::LoadAccelerators( hInstance, MAKEINTRESOURCE( IDR_ACC1 ) );
    MSG msg;
    while( ::GetMessage( &msg, NULL, 0, 0 ) > 0 ) {
        if( !::IsDialogMessage( win.GetHandleDialog(), &msg ) && !::TranslateAccelerator( win.GetHandle(), table, &msg ) ) {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
        }
    }

    return ( int ) msg.wParam;
}