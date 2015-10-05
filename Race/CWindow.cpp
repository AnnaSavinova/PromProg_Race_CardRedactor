#include "stdafx.h"
#include <string>
#include <fstream>
#include <Commdlg.h>
#include <atlbase.h>

extern HRESULT InitRibbon(HWND hWindowFrame);
extern void DestroyRibbon();

static LPCWSTR szWindowClass = L"CWindow";

HWND CWindow::GetHandle()
{
    return handle;
}

HWND CWindow::GetHandleDialog()
{
    return handleDlg;
}

bool CWindow::Create()
{
    handle = ::CreateWindow( szWindowClass, L"Редактор карт", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_EX_LAYERED,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, ::GetModuleHandle( 0 ), this );
	InitRibbon(handle);
	::UpdateWindow( handle );
    return ( handle != 0 );
}

void CWindow::Initialize()
{
    if( !loadedFromFile ) {
        numbers = std::vector< std::vector<int> >( sizeY );
        for( int i = 0; i < sizeY; i++ ) {
            numbers[i].resize( sizeX );
            for( int j = 0; j < sizeX; j++ ) {
                numbers[i][j] = 0;
            }
        }
    }
    OnSize();
}

CWindow::CWindow()
{
    backgroundBrush = ::CreateSolidBrush( RGB( 0xFF, 0xFF, 0xFF ) );

	HINSTANCE hInst = ::GetModuleHandle(0);
	HBITMAP forest = ::LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FOREST));
    brushes.push_back( ::CreatePatternBrush( forest ) );
	HBITMAP road = ::LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ROAD));
	brushes.push_back(::CreatePatternBrush(road)); 
	HBITMAP start = ::LoadBitmap(hInst, MAKEINTRESOURCE(IDB_START));
	brushes.push_back(::CreatePatternBrush(start));

    sizeX = 8;
    sizeY = 6;
    loadedFromFile = false;
    StartNewGame();
}

CWindow::~CWindow() {}

bool CWindow::RegisterClass()
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = 0;
    wcex.lpfnWndProc = windowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    HINSTANCE hInstance = ::GetModuleHandle( 0 );
    wcex.hInstance = hInstance;
    wcex.hIcon = reinterpret_cast< HICON >( ::LoadImage( hInstance, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0 ) );
    wcex.hCursor = ::LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = reinterpret_cast< HBRUSH >( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = reinterpret_cast< LPCWSTR >( MAKEINTRESOURCE( IDR_MENU1 ) );
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = reinterpret_cast< HICON >( ::LoadImage( hInstance, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0 ) );;

    return ::RegisterClassEx( &wcex );
}

void CWindow::Show( int cmdShow )
{
    ShowWindow( handle, cmdShow );
}

void CWindow::OnDestroy()
{
	DestroyRibbon();
	::PostQuitMessage( 0 );
}

void CWindow::OnSize()
{
    RECT rect;
    RECT windowRect;
    ::GetClientRect( handle, &rect );
    ::GetWindowRect( handle, &windowRect );
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int diffHeight = windowHeight - height;
    int diffWidth = windowWidth - width;
    if( ( height / sizeY ) > 30 ) {
        cellSize = height / sizeY;
    }
    ::SetWindowPos( handle, NULL, windowRect.left, windowRect.top, diffWidth + cellSize * sizeX, diffHeight + cellSize * sizeY, NULL );
    ::InvalidateRect( handle, &rect, TRUE );
}

void CWindow::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint( handle, &ps );

    RECT rect;
    ::GetClientRect( handle, &rect );

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    HDC backbuffDC = ::CreateCompatibleDC( hdc );
    HBITMAP backbuffer = ::CreateCompatibleBitmap( hdc, width, height );
    HGDIOBJ oldBitmap = ::SelectObject( backbuffDC, backbuffer );

    ::SelectObject( backbuffDC, backgroundBrush );
    ::Rectangle( backbuffDC, 0, 0, width, height );

    for( int i = 0; i < sizeY; i++ ) {
        for( int j = 0; j < sizeX; j++ ) {
            RECT rect;
            rect.left = j * cellSize;
            rect.top = i * cellSize;
            rect.right = ( j + 1 ) * cellSize;
            rect.bottom = ( i + 1 ) * cellSize;

            ::SelectObject( backbuffDC, brushes[numbers[i][j]] );
            ::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
        }
    }
    ::BitBlt( hdc, 0, 0, width, height, backbuffDC, 0, 0, SRCCOPY );

    ::SelectObject( backbuffDC, oldBitmap );
    ::DeleteObject( backbuffer );
    ::DeleteDC( backbuffDC );
    ::ReleaseDC( handle, hdc );

    ::EndPaint( handle, &ps );
}

void CWindow::StartNewGame()
{
    Initialize();
    RECT rect;
    ::GetClientRect( handle, &rect );
    ::InvalidateRect( handle, &rect, TRUE );
}

void CWindow::LoadFile()
{
    wchar_t szFilePathName[_MAX_PATH] = L"";
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = GetHandle();
    ofn.lpstrFilter = L"Race map files (*.rcmap)\0*.rcmap\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = szFilePathName;
    ofn.lpstrDefExt = L"rcmap";
    ofn.nMaxFile = _MAX_PATH;
    ofn.lpstrTitle = L"Load Map";
    ofn.Flags = OFN_FILEMUSTEXIST;

    ::GetOpenFileName( &ofn );

    std::ifstream fin;
    fin.open( ofn.lpstrFile );

    if( fin.is_open() ) {
        fin >> sizeX >> sizeY;
        numbers.resize( sizeY );
        for( int i = 0; i < sizeY; i++ ) {
            numbers[i].resize( sizeX );
            for( int j = 0; j < sizeX; j++ ) {
                fin >> numbers[i][j];
            }
        }

        loadedFromFile = true;
        Initialize();
        RECT rect;
        ::GetClientRect( handle, &rect );
        ::InvalidateRect( handle, &rect, TRUE );
    }
    fin.close();
}

void CWindow::SaveFile()
{
    wchar_t szFilePathName[_MAX_PATH] = L"";
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof( OPENFILENAME );
    ofn.hwndOwner = GetHandle();
    ofn.lpstrFilter = L"Race map files (*.rcmap)\0*.rcmap\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = szFilePathName;
    ofn.lpstrDefExt = L"rcmap";
    ofn.nMaxFile = _MAX_PATH;
    ofn.lpstrTitle = L"Save Map";
    ofn.Flags = OFN_OVERWRITEPROMPT;

    ::GetSaveFileName( &ofn );

    std::ofstream fout;
    fout.open( ofn.lpstrFile );
    if( fout.is_open() ) {
        fout << sizeX << " " << sizeY;
        fout << std::endl;

        for( int i = 0; i < sizeY; i++ ) {
            for( int j = 0; j < sizeX; j++ ) {
                fout << numbers[i][j] << " ";
            }
            fout << std::endl;
        }
    }
    fout.close();
}

void CWindow::OnClose()
{
    switch( MessageBox( handle, L"Вы уверены?", L"Выйти из редактора", MB_YESNO | MB_ICONQUESTION ) ) {
        case IDNO:
            return;
        case IDYES:
            ::PostQuitMessage( 0 );
            break;
    }
}

void CWindow::OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if( HIWORD( wParam ) == 0 ) {
        switch( LOWORD( wParam ) ) {
            case ID_EXIT:
                OnClose();
                break;
            case ID_NEWGAME:
                ::CreateDialog( GetModuleHandle( 0 ), MAKEINTRESOURCE( IDD_DIALOG1 ), handle, dialogProc );
                break;
            case ID_SAVE:
                SaveFile();
                break;
            case ID_LOAD:
                LoadFile();
                break;
            case ID_CLEAR:
                for( int i = 0; i < sizeY; i++ ) {
                    for( int j = 0; j < sizeX; j++ ) {
                        numbers[i][j] = 0;
                    }
                }
                RECT rect;
                ::GetClientRect( handle, &rect );
                ::InvalidateRect( handle, &rect, TRUE );
                break;
            default:
                break;
        }
    }
}

INT_PTR __stdcall CWindow::dialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    CWindow* that = reinterpret_cast< CWindow* >( ::GetWindowLong( ::GetParent( hwndDlg ), GWL_USERDATA ) );

    switch( uMsg ) {
        case WM_INITDIALOG:
            that->handleDlg = hwndDlg;
            return TRUE;
        case WM_COMMAND:
            switch( LOWORD( wParam ) ) {
                case IDOK:
                    int newSizeX, newSizeY;
                    newSizeY = ::GetDlgItemInt( hwndDlg, IDC_EDIT_HEIGHT, NULL, false );
                    newSizeX = ::GetDlgItemInt( hwndDlg, IDC_EDIT_WIDTH, NULL, false );

                    switch( ::MessageBox( that->handle, L"Создать новую карту?", L"Новая карта", MB_YESNO | MB_ICONWARNING ) ) {
                        case IDYES:
                            that->sizeX = newSizeX;
                            that->sizeY = newSizeY;
                            that->loadedFromFile = false;
                            that->StartNewGame();
                            break;
                        case IDNO:
                            break;
                    }
                    ::DestroyWindow( hwndDlg );
                    return TRUE;
                case IDCANCEL:
                    ::DestroyWindow( hwndDlg );
                    return TRUE;
            }
            break;
        default:
            return FALSE;
    }

    return TRUE;
}


void CWindow::OnClick( LPARAM lParam )
{
    RECT rect;
    ::GetClientRect( handle, &rect );

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int xPos = GET_X_LPARAM( lParam );
    int yPos = GET_Y_LPARAM( lParam );
    int mouseI = yPos / cellSize;
    int mouseJ = xPos / cellSize;
    numbers[mouseI][mouseJ] = ( numbers[mouseI][mouseJ] + 1 ) % brushes.size();

    ::InvalidateRect( handle, &rect, TRUE );
}

LRESULT __stdcall CWindow::windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    CWindow* that = reinterpret_cast< CWindow* >( ::GetWindowLong( hWnd, GWL_USERDATA ) );
    switch( message ) {
        case WM_NCCREATE:
        {
            CREATESTRUCT* str = reinterpret_cast< CREATESTRUCT* >( lParam );
            ::SetWindowLong( hWnd, GWL_USERDATA, LONG( str->lpCreateParams ) );
            return ::DefWindowProc( hWnd, message, wParam, lParam );
        }
        case WM_PAINT:
            that->OnPaint();
            return 0;
        case WM_CLOSE:
            that->OnClose();
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_SIZE:
            that->OnSize();
            return 0;
        case WM_DESTROY:
            that->OnDestroy();
            return 0;
        case WM_COMMAND:
            that->OnCommand( hWnd, message, wParam, lParam );
            return 0;
        case WM_LBUTTONDOWN:
            that->OnClick( lParam );
            return 0;
        default:
            return ::DefWindowProc( hWnd, message, wParam, lParam );
    }
}
