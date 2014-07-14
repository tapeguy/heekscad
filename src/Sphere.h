// Sphere.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Solid.h"

class CSphere: public CSolid{
protected:
	// CShape's virtual functions
	void MakeTransformedShape(const gp_Trsf &mat);
	wxString StretchedName();

public:
	PropertyVertex m_pos;
	PropertyLength m_radius;

	CSphere(const gp_Pnt& pos, double radius, const wxChar* title, const HeeksColor& col, float opacity);
	CSphere(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity);
	CSphere(const CSphere & rhs);
	CSphere & operator= ( const CSphere &rhs );

	// HeeksObj's virtual functions
	void InitializeProperties();
	const wxChar* GetTypeString(void)const{return _("Sphere");}
	const wxBitmap &GetIcon();
	HeeksObj *MakeACopy(void)const;
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void OnPropertyEdit(Property& prop);
	bool GetCentrePoint(double* pos);
	bool GetScaleAboutMatrix(double *m);
	bool IsDifferent(HeeksObj* other);
	bool DescendForUndo(){return false;}

	// CShape's virtual functions
	void SetXMLElement(TiXmlElement* element);
	void SetFromXMLElement(TiXmlElement* pElem);

	// CSolid's virtual functions
	SolidTypeEnum GetSolidType(){return SOLID_TYPE_SPHERE;}
};
