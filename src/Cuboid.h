// Cuboid.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Solid.h"


class CCuboid: public CSolid
{
private:
    bool in_set;

protected:
	// CShape's virtual functions
	void MakeTransformedShape(const gp_Trsf &mat);
	wxString StretchedName();

public:
	PropertyCoord m_pos;
	PropertyLength m_x; // width
	PropertyLength m_y; // height
	PropertyLength m_z; // depth

	CCuboid(const gp_Ax2& pos, double x, double y, double z, const wxChar* title, const HeeksColor& col, float opacity);
	CCuboid(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity);
	CCuboid(const CCuboid & rhs);
	CCuboid& operator= ( const CCuboid &rhs );

	// HeeksObj's virtual functions
	void InitializeProperties();
	bool IsDifferent(HeeksObj* other);
	const wxChar* GetTypeString(void)const{return _("Cuboid");}
	const wxBitmap &GetIcon();
	HeeksObj *MakeACopy(void)const;
	void GetProperties(std::list<Property *> *list);
	//ObjectCanvas* GetDialog(wxWindow* parent);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void OnPropertySet(Property& prop);
	bool GetScaleAboutMatrix(double *m);
	bool Stretch(const double *p, const double* shift, void* data);
	bool DescendForUndo(){return false;}

	// CShape's virtual functions
	void SetXMLElement(TiXmlElement* element);
	void SetFromXMLElement(TiXmlElement* pElem);

	// CSolid's virtual functions
	SolidTypeEnum GetSolidType(){return SOLID_TYPE_CUBOID;}
};
