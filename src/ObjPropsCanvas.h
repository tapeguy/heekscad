// ObjPropsCanvas.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "PropertiesCanvas.h"

class CObjPropsCanvas: public CPropertiesCanvas
{
private:
	wxAuiToolBar *m_toolBar;
	wxChoice *m_unit_dropdown;

	void ClearInitialProperties();
	void RefreshByRemovingAndAddingAll();
	void RefreshByMerging();

public:
	CObjPropsCanvas(wxWindow* parent);
	virtual ~CObjPropsCanvas();

	//virtual void OnDraw(wxDC& dc);
	void OnSize(wxSizeEvent& event);
	void OnPropertyGridChange( wxPropertyGridEvent& event );
	void OnPropertyEditBegin( wxPropertyGridEvent& event );
    void OnBindButton(wxCommandEvent& event);

	// Observer's virtual functions
	void WhenMarkedListChanges(bool selection_cleared, const std::list<HeeksObj *>* added_list, const std::list<HeeksObj *>* removed_list);
	void OnChanged(const std::list<HeeksObj*>* added, const std::list<HeeksObj*>* removed, const std::list<HeeksObj*>* modified);

	// CPropertiesCanvas's virtual functions
	void RefreshProperties2();

	void AddToolBar();

	void Resize();

    DECLARE_NO_COPY_CLASS(CObjPropsCanvas)
    DECLARE_EVENT_TABLE()
};
