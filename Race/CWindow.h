#pragma once

#include "stdafx.h"
#include "resource.h"
#include <vector>
#include "toolbar.h"
#include "CCommandHandler.h"

class CApplicationInterface : public IUIApplication, public CComObjectRootEx<CComSingleThreadModel>
{

public:
	BEGIN_COM_MAP( CApplicationInterface )
		COM_INTERFACE_ENTRY( IUIApplication )
	END_COM_MAP()

};

class CWindow : public CApplicationInterface
{
public:
	friend CCommandHandler;

	CWindow();
	~CWindow();

	static bool RegisterClass();
	bool Create();
	void Show( int cmdShow );
	HWND GetHandle();
	HWND GetHandleDialog();

	void SetHandle( HWND handle );
	void StartNewGame();


	// Static method to create an instance of the object.
	static HRESULT CreateInstance( __deref_out IUIApplication **ppApplication );

	// IUnknown methods.
	STDMETHOD_( ULONG, AddRef() );
	STDMETHOD_( ULONG, Release() );
	STDMETHOD( QueryInterface( REFIID iid, void** ppv ) );

	// IUIApplication methods
	STDMETHOD( OnCreateUICommand )( UINT nCmdID,
		__in UI_COMMANDTYPE typeID,
		__deref_out IUICommandHandler** ppCommandHandler );

	STDMETHOD( OnViewChanged )( UINT viewId,
		__in UI_VIEWTYPE typeId,
		__in IUnknown* pView,
		UI_VIEWVERB verb,
		INT uReasonCode );

	STDMETHOD( OnDestroyUICommand )( UINT32 commandId,
		__in UI_COMMANDTYPE typeID,
		__in_opt IUICommandHandler* commandHandler );

protected:
	void OnDestroy();
	void OnSize();
	void OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	void OnClose();
	void OnPaint();
	void OnClick( LPARAM lParam );
	void SaveFile();
	void LoadFile();
	static INT_PTR __stdcall dialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	HWND handle;
	HWND handleDlg;
	std::vector< std::vector<int> > numbers;
	void Initialize();
	int sizeX;
	int sizeY;
	int cellSize;
	bool loadedFromFile;
	std::vector<HBRUSH> brushes;
	HBRUSH backgroundBrush;
	int qWidth, qHeight;
	LONG m_cRef;                           // Reference count.
	IUICommandHandler* m_pCommandHandler;  // Generic Command Handler

	static LRESULT __stdcall windowProc( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
};
