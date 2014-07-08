// HeeksConfig.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#pragma once

#include <wx/config.h>
#include "Property.h"

class HeeksConfig: public wxConfig
{
public:
	HeeksConfig(const wxString& appname) : wxConfig(appname){}
	~HeeksConfig(){}

	bool Read(const wxString& key, wxString * prop);
	bool Read(const wxString& key, int * prop);
	bool Read(const wxString& key, long * prop);
	bool Read(const wxString& key, double * prop);
	bool Read(const wxString& key, bool * prop);

	bool Read(const wxString& key, wxString * prop, const wxChar* def);
	bool Read(const wxString& key, int * prop, int def);
	bool Read(const wxString& key, long * prop, long def);
	bool Read(const wxString& key, double * prop, double def);
	bool Read(const wxString& key, bool * prop, bool def);

	bool Read(const wxString& key, PropertyCheck& prop); 
	bool Read(const wxString& key, PropertyChoice& prop); 
	bool Read(const wxString& key, PropertyDouble& prop);
	bool Read(const wxString& key, PropertyInt& prop); 
	bool Read(const wxString& key, PropertyString& prop);
	bool Read(const wxString& key, PropertyFile& prop);
	bool Read(const wxString& key, PropertyLength& prop);

	bool Read(const wxString& key, PropertyCheck& prop, bool def); 
	bool Read(const wxString& key, PropertyChoice& prop, int def); 
	bool Read(const wxString& key, PropertyDouble& prop, double def);
	bool Read(const wxString& key, PropertyInt& prop, int def); 
	bool Read(const wxString& key, PropertyString& prop, const wxChar* def);
	bool Read(const wxString& key, PropertyFile& prop, const wxChar* def);
	bool Read(const wxString& key, PropertyLength& prop, double def);

	bool Write(const wxString& key, const wxString& str);
	bool Write(const wxString& key, int i);
	bool Write(const wxString& key, long l);
	bool Write(const wxString& key, double d);
	bool Write(const wxString& key, bool b);

	bool Write(const wxString& key, PropertyCheck prop); 
	bool Write(const wxString& key, PropertyChoice prop); 
	bool Write(const wxString& key, PropertyDouble prop);
	bool Write(const wxString& key, PropertyInt prop); 
	bool Write(const wxString& key, PropertyString prop);
	bool Write(const wxString& key, PropertyFile prop);
	bool Write(const wxString& key, PropertyLength prop);
};
