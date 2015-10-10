#include "stdafx.h"
#include "CCommandHandler.h"
#include "toolbar.h"
#include "Ribbon.h"
#include "CWindow.h"
#include <Commdlg.h>

extern CWindow MainWindow;

// Static method to create an instance of the object.
__checkReturn HRESULT CCommandHandler::CreateInstance( __deref_out IUICommandHandler **ppCommandHandler )
{
	if ( !ppCommandHandler ) {
		return E_POINTER;
	}

	*ppCommandHandler = NULL;
	HRESULT hr = S_OK;

	CCommandHandler* pCommandHandler = new CCommandHandler();

	if ( pCommandHandler != NULL ) {
		*ppCommandHandler = static_cast<IUICommandHandler*>( pCommandHandler );
	} else {
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

// IUnknown method implementations.
STDMETHODIMP_( ULONG ) CCommandHandler::AddRef()
{
	return InterlockedIncrement( &m_cRef );
}

STDMETHODIMP_( ULONG ) CCommandHandler::Release()
{
	LONG cRef = InterlockedDecrement( &m_cRef );
	if ( cRef == 0 ) {
		delete this;
	}

	return cRef;
}

STDMETHODIMP CCommandHandler::QueryInterface( REFIID iid, void** ppv )
{
	if ( iid == __uuidof( IUnknown ) ) {
		*ppv = static_cast<IUnknown*>( this );
	} else if ( iid == __uuidof( IUICommandHandler ) ) {
		*ppv = static_cast<IUICommandHandler*>( this );
	} else {
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}


STDMETHODIMP CCommandHandler::UpdateProperty(
	UINT nCmdID,
	__in REFPROPERTYKEY key,
	__in_opt const PROPVARIANT* ppropvarCurrentValue,
	__out PROPVARIANT* ppropvarNewValue )
{
	UNREFERENCED_PARAMETER( nCmdID );
	UNREFERENCED_PARAMETER( key );
	UNREFERENCED_PARAMETER( ppropvarCurrentValue );
	UNREFERENCED_PARAMETER( ppropvarNewValue );

	return E_NOTIMPL;
}


STDMETHODIMP CCommandHandler::Execute(
	UINT nCmdID,
	UI_EXECUTIONVERB verb,
	__in_opt const PROPERTYKEY* key,
	__in_opt const PROPVARIANT* ppropvarValue,
	__in_opt IUISimplePropertySet* pCommandExecutionProperties )
{
	UNREFERENCED_PARAMETER( pCommandExecutionProperties );
	UNREFERENCED_PARAMETER( ppropvarValue );
	UNREFERENCED_PARAMETER( key );
	UNREFERENCED_PARAMETER( verb );
	UNREFERENCED_PARAMETER( nCmdID );

	switch ( nCmdID ) {
	case cmdNewMap:
		::CreateDialog( GetModuleHandle( 0 ), MAKEINTRESOURCE( IDD_DIALOG1 ), MainWindow.GetHandle(), CWindow::dialogProc );
		break;
	case cmdLoadMap:
		MainWindow.LoadFile();
		break;
	case cmdSaveMap:
		MainWindow.SaveFile();
		break;
	default:
		break;
	}

	return S_OK;
}
