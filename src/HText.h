// HText.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"
#include "CxfFont.h"

class HText: public ObjList
{
private:
    PropertyColor m_color;

public:
	PropertyTrsf m_trsf;	// matrix defining position, orientation, scale, compared with default text size
	PropertyChoice m_font;
	wxString m_text;
	VectorFont *m_pFont;	// NULL for internal (normal) fonts

	HText(const gp_Trsf &trsf, const wxString &text, const HeeksColor& col, VectorFont *pVectorFont);
	HText(const HText &b);
	~HText(void);

	const HText& operator=(const HText &b);

	// HeeksObj's virtual functions
	void InitializeProperties();
    void OnPropertyEdit(Property& prop);
	int GetType()const{return TextType;}
	int GetMarkingFilter()const{return TextMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	bool DrawAfterOthers(){return true;}
	void GetBox(CBox &box);
	const wxChar* GetTypeString(void)const{return _("Text");}
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool Stretch(const double *p, const double* shift, void* data);
	void CopyFrom(const HeeksObj* object){operator=(*((HText*)object));}
	void WriteXML(TiXmlNode *root);
	const wxChar* GetShortString(void)const{return m_text.c_str();}
	bool CanEditString(void)const{return true;}
	void OnEditString(const wxChar* str);
	bool CanAdd(HeeksObj* object);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	bool GetTextSize( const wxString & text, float *pWidth, float *pHeight ) const;
};
