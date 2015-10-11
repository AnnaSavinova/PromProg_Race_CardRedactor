#include "stdafx.h"
#include <Commdlg.h>
#include "Ribbon.h"
#include "CWindow.h"
#include "CCommandHandler.h"

static LPCWSTR szWindowClass = L"CWindow";

HWND CWindow::GetHandle()
{
    return handle;
}

HWND CWindow::GetHandleDialog()
{
    return handleDlg;
}

void CWindow::SetHandle( HWND newHandle )
{
    handle = newHandle;
}

bool CWindow::Create()
{
    handle = ::CreateWindow( szWindowClass, L"Редактор карт", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_EX_LAYERED,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, ::GetModuleHandle( 0 ), this );
    InitRibbon( handle );
    ::UpdateWindow( handle );

    map.RestartMap();
    UpdateState();
    return ( handle != 0 );
}

CWindow::CWindow()
{
    backgroundBrush = ::CreateSolidBrush( RGB( 0xFF, 0xFF, 0xFF ) );

    HINSTANCE hInst = ::GetModuleHandle( 0 );
    HBITMAP forest = ::LoadBitmap( hInst, MAKEINTRESOURCE( IDB_FOREST ) );
    brushes.push_back( ::CreatePatternBrush( forest ) );
    HBITMAP road = ::LoadBitmap( hInst, MAKEINTRESOURCE( IDB_ROAD ) );
    brushes.push_back( ::CreatePatternBrush( road ) );
    HBITMAP start = ::LoadBitmap( hInst, MAKEINTRESOURCE( IDB_START ) );
    brushes.push_back( ::CreatePatternBrush( start ) );
	HBITMAP wall = ::LoadBitmap( hInst, MAKEINTRESOURCE( IDB_WALL ) );
	brushes.push_back( ::CreatePatternBrush( wall ) );

    m_cRef = 1;
    m_pCommandHandler = NULL;
}

CWindow::~CWindow()
{
    if( m_pCommandHandler ) {
        m_pCommandHandler->Release();
        m_pCommandHandler = NULL;
    }

    for( size_t i = 0; i < brushes.size(); ++i ) {
        if( brushes[i] ) {
            ::DeleteBrush( brushes[i] );
        }
    }
}

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

UINT32 getRibbonHeight()
{
    UINT32 ribbonHeight;
    HRESULT hr = E_FAIL;
    IUIRibbon* pRibbon;
    hr = g_pFramework->GetView( 0, IID_PPV_ARGS( &pRibbon ) );
    if( SUCCEEDED( hr ) ) {
        UINT32 uRibbonHeight = 0;
        hr = pRibbon->GetHeight( &uRibbonHeight );
        if( SUCCEEDED( hr ) ) {
            ribbonHeight = uRibbonHeight;
        }
        pRibbon->Release();
    }
    if( FAILED( hr ) ) {
        ribbonHeight = 0;
    }

    return ribbonHeight;
}


void CWindow::OnSize()
{
    RECT rect;
    RECT windowRect;
    ::GetClientRect( handle, &rect );
    ::GetWindowRect( handle, &windowRect );
    UINT32 ribbonHeight = getRibbonHeight();
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top - ribbonHeight;
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    int diffHeight = windowHeight - height;
    int diffWidth = windowWidth - width;

    cellSize = height / map.GetY();
    //подгоняем размеры поля под высоту клеток
    ::SetWindowPos( handle, NULL, windowRect.left, windowRect.top, windowWidth, diffHeight + cellSize * map.GetY(), NULL );
    ::InvalidateRect( handle, &rect, TRUE );
}


void CWindow::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint( handle, &ps );

    RECT rect;
    ::GetClientRect( handle, &rect );

    UINT32 ribbonHeight = getRibbonHeight();

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top - ribbonHeight;

    //центрируем карту в окне
    int leftMargin = ( width - cellSize * map.GetX() ) / 2;

    HDC backbuffDC = ::CreateCompatibleDC( hdc );
    HBITMAP backbuffer = ::CreateCompatibleBitmap( hdc, width, height + ribbonHeight );
    HGDIOBJ oldBitmap = ::SelectObject( backbuffDC, backbuffer );

    ::SelectObject( backbuffDC, backgroundBrush );
    ::Rectangle( backbuffDC, 0, 0, width, height + ribbonHeight );

    for( int i = 0; i < map.GetY(); i++ ) {
        for( int j = 0; j < map.GetX(); j++ ) {
            RECT rect;
            rect.left = leftMargin + j * cellSize;
            rect.top = i * cellSize + ribbonHeight;
            rect.right = leftMargin + ( j + 1 ) * cellSize;
            rect.bottom = ( i + 1 ) * cellSize + ribbonHeight;

            ::SelectObject( backbuffDC, brushes[map.GetNumbers()[i][j]] );
            ::Rectangle( backbuffDC, rect.left, rect.top, rect.right, rect.bottom );
        }
    }
    ::BitBlt( hdc, 0, 0, width, height + ribbonHeight, backbuffDC, 0, 0, SRCCOPY );

    ::SelectObject( backbuffDC, oldBitmap );
    ::DeleteObject( backbuffer );
    ::DeleteDC( backbuffDC );
    ::ReleaseDC( handle, hdc );

    ::EndPaint( handle, &ps );
}

void CWindow::UpdateState()
{
    OnSize();
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
        map.LoadMapFromFile( fin );
        UpdateState();
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
        map.SaveMapToFile( fout );
    }
    fout.close();
}

void CWindow::OnClose()
{
    switch( MessageBox( handle, L"Вы уверены?", L"Выйти из редактора", MB_YESNO | MB_ICONQUESTION ) ) {
        case IDNO:
            return;
        case IDYES:
            OnDestroy();
            return;
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
            {
                map.RestartMap();
                UpdateState();
            }
            break;
            default:
                break;
        }
    }
}

void CWindow::onDialogOk()
{
    int newSizeY = ::GetDlgItemInt( handleDlg, IDC_EDIT_HEIGHT, NULL, false );
    int newSizeX = ::GetDlgItemInt( handleDlg, IDC_EDIT_WIDTH, NULL, false );

    switch( ::MessageBox( handle, L"Создать новую карту?", L"Новая карта", MB_YESNO | MB_ICONWARNING ) ) {
        case IDYES:
        {
            map.SetSize( newSizeX, newSizeY );
            map.RestartMap();
            UpdateState();
        }
        break;
        case IDNO:
            break;
    }
    ::DestroyWindow( handleDlg );
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
                    that->onDialogOk();
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
    int mapWidth = cellSize * map.GetX();
    int leftMargin = ( width - mapWidth ) / 2;

    int xPos = GET_X_LPARAM( lParam ) - leftMargin;
    if( xPos > 0 && xPos < mapWidth ) {
        int yPos = GET_Y_LPARAM( lParam ) - getRibbonHeight();
        int mouseI = yPos / cellSize;
        int mouseJ = xPos / cellSize;
        map.ClickCell( mouseI, mouseJ );

        ::InvalidateRect( handle, &rect, TRUE );
    }
}

// Static method to create an instance of the object.
__checkReturn HRESULT CWindow::CreateInstance( __deref_out IUIApplication **ppApplication )
{
    if( !ppApplication ) {
        return E_POINTER;
    }

    *ppApplication = NULL;
    HRESULT hr = S_OK;
    CWindow* pApplication = new CWindow();

    if( pApplication != NULL ) {
        *ppApplication = static_cast< IUIApplication * >( pApplication );
    } else {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

// IUnknown method implementations.
STDMETHODIMP_( ULONG ) CWindow::AddRef()
{
    return InterlockedIncrement( &m_cRef );
}

STDMETHODIMP_( ULONG ) CWindow::Release()
{
    LONG cRef = InterlockedDecrement( &m_cRef );
    if( cRef == 0 ) {
        delete this;
    }

    return cRef;
}

STDMETHODIMP CWindow::QueryInterface( REFIID iid, void** ppv )
{
    if( iid == __uuidof( IUnknown ) ) {
        *ppv = static_cast< IUnknown* >( this );
    } else if( iid == __uuidof( IUIApplication ) ) {
        *ppv = static_cast< IUIApplication* >( this );
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}


STDMETHODIMP CWindow::OnCreateUICommand(
    UINT nCmdID,
    __in UI_COMMANDTYPE typeID,
    __deref_out IUICommandHandler** ppCommandHandler )
{
    UNREFERENCED_PARAMETER( typeID );
    UNREFERENCED_PARAMETER( nCmdID );

    if( NULL == m_pCommandHandler ) {
        HRESULT hr = CCommandHandler::CreateInstance( &m_pCommandHandler );
        if( FAILED( hr ) ) {
            return hr;
        }
    }

    return m_pCommandHandler->QueryInterface( IID_PPV_ARGS( ppCommandHandler ) );
}

STDMETHODIMP CWindow::OnViewChanged(
    UINT viewId,
    __in UI_VIEWTYPE typeId,
    __in IUnknown* pView,
    UI_VIEWVERB verb,
    INT uReasonCode )
{
    UNREFERENCED_PARAMETER( uReasonCode );
    UNREFERENCED_PARAMETER( viewId );

    HRESULT hr = E_NOTIMPL;

    // Checks to see if the view that was changed was a Ribbon view.
    if( UI_VIEWTYPE_RIBBON == typeId ) {
        switch( verb ) {
            // The view was newly created.
            case UI_VIEWVERB_CREATE:
                hr = S_OK;
                break;

                // The view has been resized.  For the Ribbon view, the application should
                // call GetHeight to determine the height of the ribbon.
            case UI_VIEWVERB_SIZE:
            {
                IUIRibbon* pRibbon = NULL;
                UINT uRibbonHeight;

                hr = pView->QueryInterface( IID_PPV_ARGS( &pRibbon ) );
                if( SUCCEEDED( hr ) ) {
                    // Call to the framework to determine the desired height of the Ribbon.
                    hr = pRibbon->GetHeight( &uRibbonHeight );
                    pRibbon->Release();
                    // Use the ribbon height to position controls in the client area of the window.
                }
            }
            break;
            // The view was destroyed.
            case UI_VIEWVERB_DESTROY:
                hr = S_OK;
                break;
        }
    }

    return hr;
}

STDMETHODIMP CWindow::OnDestroyUICommand(
    UINT32 nCmdID,
    __in UI_COMMANDTYPE typeID,
    __in_opt IUICommandHandler* commandHandler )
{
    UNREFERENCED_PARAMETER( commandHandler );
    UNREFERENCED_PARAMETER( typeID );
    UNREFERENCED_PARAMETER( nCmdID );

    return E_NOTIMPL;
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
