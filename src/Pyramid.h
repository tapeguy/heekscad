// Cuboid.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Solid.h"


class CPyramid: public CSolid
{
protected:
	// CShape's virtual functions
	void MakeTransformedShape(const gp_Trsf &mat);
	wxString StretchedName();

public:
	PropertyCoord m_pos;
	PropertyInt m_sides;
	PropertyLength m_length;      // length of each base side
	PropertyLength m_height;

	CPyramid(const gp_Ax2& pos, int sides, double length, double height, const wxChar* title, const HeeksColor& col, float opacity);
	CPyramid(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity);
	CPyramid(const CPyramid & rhs);
	CPyramid& operator= ( const CPyramid &rhs );

	// HeeksObj's virtual functions
	void InitializeProperties();
	bool IsDifferent(HeeksObj* other);
	const wxChar* GetTypeString(void)const{return _("Pyramid");}
	const wxBitmap &GetIcon();
	HeeksObj *MakeACopy(void)const;
	void GetProperties(std::list<Property *> *list);
	//ObjectCanvas* GetDialog(wxWindow* parent);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void OnPropertyEdit(Property& prop);
	bool GetScaleAboutMatrix(double *m);
	bool DescendForUndo(){return false;}

	// CSolid's virtual functions
	SolidTypeEnum GetSolidType(){return SOLID_TYPE_PYRAMID;}
};
