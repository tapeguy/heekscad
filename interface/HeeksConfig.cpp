
#include "stdafx.h"
#include "HeeksConfig.h"

bool HeeksConfig::HeeksConfig::Read(const wxString& key, wxString * prop) {
	return wxConfig::Read(key, prop);
}

bool HeeksConfig::HeeksConfig::Read(const wxString& key, int * prop) {
	return wxConfig::Read(key, prop);
}

bool HeeksConfig::HeeksConfig::Read(const wxString& key, long * prop) {
	return wxConfig::Read(key, prop);
}

bool HeeksConfig::HeeksConfig::Read(const wxString& key, double * prop) {
	return wxConfig::Read(key, prop);
}

bool HeeksConfig::Read(const wxString& key, bool * prop) {
	return wxConfig::Read(key, prop);
}

bool HeeksConfig::Read(const wxString& key, wxString * prop, const wxChar* def) {
	return wxConfig::Read(key, prop, def);
}

bool HeeksConfig::Read(const wxString& key, int * prop, int def) {
	return wxConfig::Read(key, prop, def);
}

bool HeeksConfig::Read(const wxString& key, long * prop, long def) {
	return wxConfig::Read(key, prop, def);
}

bool HeeksConfig::Read(const wxString& key, double * prop, double def) {
	return wxConfig::Read(key, prop, def);
}

bool HeeksConfig::Read(const wxString& key, bool * prop, bool def) {
	return wxConfig::Read(key, prop, def);
}

bool HeeksConfig::Read(const wxString& key, PropertyCheck& prop) {
	bool value = prop.IsSet();
	bool rtn = wxConfig::Read(key, &value);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyChoice& prop) {
	int value = prop;
	bool rtn = wxConfig::Read(key, &value);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyDouble& prop) {
	double value = prop;
	bool rtn = wxConfig::Read(key, &value);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyInt& prop) {
	int value = prop;
	bool rtn = wxConfig::Read(key, &value);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyString& prop) {
	wxString value = prop;
	bool rtn = wxConfig::Read(key, &value);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyFile& prop) {
	wxString value = prop;
	bool rtn = wxConfig::Read(key, &value);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyLength& prop) {
	double value = prop;
	bool rtn = wxConfig::Read(key, &value);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyCheck& prop, bool def) {
	bool value = prop.IsSet();
	bool rtn = wxConfig::Read(key, &value, def);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyChoice& prop, int def) {
	int value = prop;
	bool rtn = wxConfig::Read(key, &value, def);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyDouble& prop, double def) {
	double value = prop;
	bool rtn = wxConfig::Read(key, &value, def);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyInt& prop, int def) {
	int value = prop;
	bool rtn = wxConfig::Read(key, &value, def);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyString& prop, const wxChar* def) {
	wxString value = prop;
	bool rtn = wxConfig::Read(key, &value, def);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyFile& prop, const wxChar* def) {
	wxString value = prop;
	bool rtn = wxConfig::Read(key, &value, def);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Read(const wxString& key, PropertyLength& prop, double def) {
	double value = prop;
	bool rtn = wxConfig::Read(key, &value, def);
	prop.SetValue(value);
	return rtn;
}

bool HeeksConfig::Write(const wxString& key, const wxString& str) {
	return wxConfig::Write(key, str);
}

bool HeeksConfig::Write(const wxString& key, int i) {
	return wxConfig::Write(key, i);
}

bool HeeksConfig::Write(const wxString& key, long l) {
	return wxConfig::Write(key, l);
}

bool HeeksConfig::Write(const wxString& key, double d) {
	return wxConfig::Write(key, d);
}

bool HeeksConfig::Write(const wxString& key, bool b) {
	return wxConfig::Write(key, b);
}

bool HeeksConfig::Write(const wxString& key, PropertyCheck prop) {
	bool value = prop.IsSet();
	return wxConfig::Write(key, value);
}

bool HeeksConfig::Write(const wxString& key, PropertyChoice prop) {
	return wxConfig::Write(key, (int)prop);
}

bool HeeksConfig::Write(const wxString& key, PropertyDouble prop) {
	return wxConfig::Write(key, (double)prop);
}

bool HeeksConfig::Write(const wxString& key, PropertyInt prop) {
	return wxConfig::Write(key, (int)prop);
}

bool HeeksConfig::Write(const wxString& key, PropertyString prop) {
	return wxConfig::Write(key, (const wxString&)prop);
}

bool HeeksConfig::Write(const wxString& key, PropertyFile prop) {
	return wxConfig::Write(key, (const wxString&)prop);
}

bool HeeksConfig::Write(const wxString& key, PropertyLength prop) {
	return wxConfig::Write(key, (double)prop);
}

