// Cylinder.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Solid.h"

class CCylinder: public CSolid{
protected:
	// CShape's virtual functions
	void MakeTransformedShape(const gp_Trsf &mat);
	wxString StretchedName();

public:

	PropertyCoord m_pos;
	PropertyLength m_diameter;
	PropertyLength m_height;

	CCylinder(const gp_Ax2& pos, double radius, double height, const wxChar* title, const HeeksColor& col, float opacity);
	CCylinder(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity);

    void InitializeProperties();
    void OnPropertySet(Property& prop);

	// HeeksObj's virtual functions
	bool IsDifferent(HeeksObj* other);
	const wxChar* GetTypeString(void)const{return _("Cylinder");}
	const wxBitmap &GetIcon();
	HeeksObj *MakeACopy(void)const;
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	int GetCentrePoints(double* pos, double* pos2);
	bool GetScaleAboutMatrix(double *m);
	bool Stretch(const double *p, const double* shift, void* data);
	bool DescendForUndo(){return false;}

	// CShape's virtual functions
	void SetXMLElement(TiXmlElement* element);
	void SetFromXMLElement(TiXmlElement* pElem);

	// CSolid's virtual functions
	SolidTypeEnum GetSolidType(){return SOLID_TYPE_CYLINDER;}
};
