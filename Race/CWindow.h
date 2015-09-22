#pragma once

#include "resource.h"
#include <vector>

class CWindow {
public:
    CWindow();
    ~CWindow();

    static bool RegisterClass();
    bool Create();
    void Show( int cmdShow );
    HWND GetHandle();
    HWND GetHandleDialog();
    void StartNewGame();

protected:
    void OnDestroy();
    void OnSize( LPARAM lparam );
    void OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
    void OnClose();
    void SaveFile();
    void OnPaint();
    void OnClick( LPARAM lParam );

private:
    HWND handle;
    HWND handleDlg;
    std::vector< std::vector<int> > numbers;
    void Initialize();
    void LoadFile();
    size_t sizeX;
    size_t sizeY;
    bool loadedFromFile;
    std::vector<HBRUSH> brushes;
    HBRUSH backgroundBrush;
    int qWidth, qHeight;

    static INT_PTR __stdcall dialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
    static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
};
