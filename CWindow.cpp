#include "stdafx.h"
#include <string>

static TCHAR szWindowClass[] = _T( "CWindow" );

HWND CWindow::GetHandle() {
    return handle;
}

HWND CWindow::GetHandleDialog() {
    return handleDlg;
}

bool CWindow::Create() {
    handle = ::CreateWindow( szWindowClass, L"Редактор карт", WS_OVERLAPPEDWINDOW  | WS_CLIPCHILDREN | WS_EX_LAYERED,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, ::GetModuleHandle( 0 ), this );
	::UpdateWindow( handle );
    return ( handle != 0 );
}

void CWindow::Initialize() {
	if ( !loadedFromFile ) {
		numbers = std::vector< std::vector<int> >( sizeY );
		for ( size_t i = 0; i < sizeY; i++ ) {
			numbers[i].resize( sizeX );
			for ( size_t j = 0; j < sizeX; j++ ) {
				numbers[i][j] = 0;
			}
		}
	}
	RECT rect;
	::GetClientRect( handle, &rect ); 
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

    qWidth = ( width - 5 ) / ( sizeX ) + 1;
    qHeight = ( height - 5 ) / ( sizeY ) + 1;

	for( size_t i = 0; i < sizeY; i++ ) {
        for( size_t j = 0; j < sizeX; j++ ) {
            RECT rect;
            rect.left = j * qWidth;
            rect.top = i * qHeight;
            rect.right = ( j + 1 ) * qWidth;
            rect.bottom = ( i + 1 ) * qHeight;
        }
    }
}

CWindow::CWindow() {
	whiteBrush = ::CreateSolidBrush( RGB( 0xFF, 0xFF, 0xFF) );
	greyBrush = ::CreateSolidBrush( RGB( 0xD0, 0xD0, 0xD0) );
	blackBrush = ::CreateSolidBrush( RGB( 0x0, 0x0, 0x0) );
    sizeX = 8;
	sizeY = 6;
	loadedFromFile = false;
    StartNewGame();
}
CWindow::~CWindow() {}

bool CWindow::RegisterClass() {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = 0;
    wcex.lpfnWndProc = windowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    HINSTANCE hInstance = ::GetModuleHandle( 0 );
    wcex.hInstance = hInstance;
    wcex.hIcon = reinterpret_cast<HICON>( ::LoadImage( hInstance, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0 ) );
    wcex.hCursor = ::LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = reinterpret_cast<LPCWSTR>( MAKEINTRESOURCE( IDR_MENU1 ) );
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = reinterpret_cast<HICON>( ::LoadImage( hInstance, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0 ) );;

    return ::RegisterClassEx( &wcex );
}

void CWindow::Show( int cmdShow ) {
    ShowWindow( handle, cmdShow );
}

void CWindow::OnDestroy() {
    ::PostQuitMessage( 0 );
}

void CWindow::OnSize( LPARAM lParam ) {
    RECT rect;
    RECT windowRect;
    ::GetClientRect( handle, &rect );
    ::GetWindowRect( handle, &windowRect );
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int diff = (windowHeight - height);
    ::SetWindowPos( handle, NULL, windowRect.left, windowRect.top, windowWidth, diff + windowWidth * sizeY / sizeX, 0 );
    ::GetClientRect( handle, &rect );
    ::InvalidateRect( handle, &rect, TRUE );
}

void CWindow::OnPaint() {
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint( handle, &ps );

	RECT rect;
	::GetClientRect( handle, &rect );

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	HDC backbuffDC = ::CreateCompatibleDC( hdc );
	HBITMAP backbuffer = ::CreateCompatibleBitmap( hdc, width, height );
	::SelectObject( backbuffDC, backbuffer );

    COLORREF back = RGB( 0, 0, 0 );
	HBRUSH backgroundHBrush = ::CreateSolidBrush( back );
	::FillRect( backbuffDC, &rect, backgroundHBrush );

    HPEN hpen = ::CreatePen(PS_SOLID, 1, back); 
    HGDIOBJ hpenOld = SelectObject( backbuffDC, hpen ); 

	::SelectObject( backbuffDC, whiteBrush );
	::SetPolyFillMode( backbuffDC, WINDING );
    qWidth = ( width - 5 ) / sizeX + 1;
    qHeight = ( height - 5 ) / sizeY + 1;

    ::SetBkMode( backbuffDC, OPAQUE );
    ::SetTextColor( backbuffDC, RGB( 255, 255, 255 ) );

    for( size_t i = 0; i < sizeY; i++ ) {
        for( size_t j = 0; j < sizeX; j++ ) {
			RECT rect;
			rect.left = j * qWidth;
			rect.top = i * qHeight;
			rect.right = ( j + 1 ) * qWidth;
			rect.bottom = ( i + 1 ) * qHeight;
            switch ( numbers[i][j] ) {
			case 0:
				::SelectObject( backbuffDC, greyBrush );
				::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
				break;
			case 1:
				::SelectObject( backbuffDC, blackBrush );
				::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );					break;
			default:
				::SelectObject( backbuffDC, whiteBrush );
				::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
			}
        }
    }
	::BitBlt( hdc, 0, 0, width, height, backbuffDC, 0, 0, SRCCOPY );

    ::SelectObject( backbuffDC, hpenOld );
    ::DeleteObject( hpen );

	::DeleteObject( backbuffer );
	::DeleteDC( backbuffDC );

	::EndPaint( handle, &ps ); 
	::DeleteObject( backgroundHBrush );
}

void CWindow::StartNewGame() {
	Initialize();
	OnSize( NULL );
    RECT rect;
    ::GetClientRect( handle, &rect );
    ::InvalidateRect( handle, &rect, TRUE );
}

void CWindow::LoadFile() {
    std::ifstream fin;
    fin.open("game.save");

    if( fin.is_open() ) {
        fin >> sizeX >> sizeY;
        numbers.resize( sizeY );
        for( size_t i = 0; i < sizeY; i++ ) {
            numbers[i].resize( sizeX );
			for( size_t j = 0; j < sizeX; j++ ) {
                fin >> numbers[i][j];
            }
        }
       
        loadedFromFile = true;
		Initialize();
		OnSize(NULL);
        RECT rect;
        ::GetClientRect( handle, &rect );
        ::InvalidateRect( handle, &rect, TRUE );
    }
    fin.close();
}

void CWindow::SaveFile() {
    //TODO
}

void CWindow::OnClose() {
	switch (MessageBox(handle, L"Вы уверены?", L"Выйти из игры", MB_YESNO | MB_ICONQUESTION)) {
	case IDNO:
		return;
	case IDYES:
		//switch( MessageBox( handle, L"И это после всего, что между нами было?!", L"Выйти из игры", MB_YESNO | MB_ICONWARNING ) ) {
		//case IDNO:
		//	return;   
		//case IDYES:
		//SaveFile();
		::PostQuitMessage(0);
		break;
	}
}

void CWindow::OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
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
			for (size_t i = 0; i < sizeY; i++) {
				for (size_t j = 0; j < sizeX; j++) {
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

INT_PTR __stdcall CWindow::dialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) { 
    CWindow* that = reinterpret_cast<CWindow*>( ::GetWindowLong( ::GetParent( hwndDlg ), GWL_USERDATA ) );

    switch( uMsg ) {
    case WM_INITDIALOG:
        that->handleDlg = hwndDlg;
        return TRUE;
    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDOK:
			int _sizeX, _sizeY, _blocksNum;
			char bufferX[10], bufferY[10], bufferBlock[10];
            GetWindowText( ::GetDlgItem( hwndDlg, IDC_EDIT_HEIGHT ), reinterpret_cast<LPWSTR>( bufferX ), sizeof( bufferX ) );
            _sizeX = atoi( bufferX );
            GetWindowText( ::GetDlgItem( hwndDlg, IDC_EDIT_WIDTH ), reinterpret_cast<LPWSTR>( bufferY ), sizeof( bufferY ) );
            _sizeY = atoi( bufferY );
            GetWindowText( ::GetDlgItem( hwndDlg, IDC_EDIT_BLOCKS ), reinterpret_cast<LPWSTR>( bufferBlock ), sizeof( bufferBlock ) );
            _blocksNum = atoi( bufferBlock );
            switch( ::MessageBox( that->handle, L"Начать новую игру?", L"Новая игра", MB_YESNO | MB_ICONWARNING ) ) {
            case IDYES:
		        that->sizeX = _sizeX;
				that->sizeY = _sizeY;
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

LRESULT CWindow::OnCtlColorEdit( HDC dc, HWND hEdit, CWindow *win ) {
	return ( LRESULT )CreateSolidBrush( RGB( 138, 255, 92 ) );
}

void CWindow::OnClick( LPARAM lParam ) {
    RECT rect;
	::GetClientRect( handle, &rect ); 

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

    int xPos = GET_X_LPARAM(lParam); 
    int yPos = GET_Y_LPARAM(lParam); 
    int mouseI = yPos / qHeight;
    int mouseJ = xPos / qWidth;
	numbers[mouseI][mouseJ] = (numbers[mouseI][mouseJ] + 1) % 3; 
	
    ::GetClientRect( handle, &rect );
    ::InvalidateRect( handle, &rect, TRUE );  
}

LRESULT __stdcall CWindow::windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    CWindow* that = reinterpret_cast<CWindow*>( ::GetWindowLong( hWnd, GWL_USERDATA ) );
    switch( message ) {
    case WM_NCCREATE: 
        {
            CREATESTRUCT* str = reinterpret_cast<CREATESTRUCT*>( lParam );
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
        that->OnSize( lParam );
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
	case WM_CTLCOLOREDIT:
		return that->OnCtlColorEdit((HDC)wParam, (HWND)lParam, that);
    default:
        return ::DefWindowProc( hWnd, message, wParam, lParam );
    }
}
