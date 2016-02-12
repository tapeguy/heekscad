// HAngularDimension.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "HPoint.h"

enum AngularDimensionTextMode
{
	StringAngularDimensionTextMode,
	DegreesAngularDimensionTextMode,
	RadiansAngularDimensionTextMode
};


class HAngularDimension: public HeeksObj
{
private:
    PropertyColor m_color;

    // Private no-arg constructor
    HAngularDimension();

public:

    static const int ObjType = AngularDimensionType;


	HPoint* m_p0;
	HPoint* m_p1;
	HPoint* m_p2;
	HPoint* m_p3;
	HPoint* m_p4;
    PropertyString m_text;
	PropertyChoice m_text_mode;
	PropertyDouble m_scale; // to do - text, gaps, and arrow heads will be scaled by this factor

	HAngularDimension(const wxString &text, const gp_Pnt &p0, const gp_Pnt &p1, const gp_Pnt &p2, const gp_Pnt &p3, const gp_Pnt &p4, AngularDimensionTextMode text_mode, const HeeksColor& col);
	HAngularDimension(const wxString &text, AngularDimensionTextMode text_mode, const HeeksColor& col);
	HAngularDimension(const HAngularDimension &b);
	~HAngularDimension(void);

	const HAngularDimension& operator=(const HAngularDimension &b);
	void DrawLine(gp_Pnt p1, gp_Pnt p2);
	void DrawArc(gp_Pnt center, double radius, double a1, double a2);

	// HeeksObj's virtual functions
	void InitializeProperties();
	int GetMarkingFilter()const{return DimensionMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	bool DrawAfterOthers(){return true;}
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool Stretch(const double *p, const double* shift, void* data);
	void CopyFrom(const HeeksObj* object){operator=(*((HAngularDimension*)object));}
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool IsDifferent(HeeksObj* other);
	void ReloadPointers();

	wxString MakeText(double angle);
	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
};
