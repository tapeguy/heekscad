// HPoint.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/IdNamedObj.h"


class HPoint : public HeeksObj
{
public:

    static const int ObjType = PointType;


	PropertyVertex m_p;
	bool m_draw_unselected;

	HPoint();
	HPoint(const gp_Pnt &p, const HeeksColor& col);
	HPoint(const HPoint &p);
    ~HPoint(void);

	const HPoint& operator=(const HPoint &b);

	// HeeksObj's virtual functions
    void InitializeProperties();
    virtual bool UsesColor() { return true; }
	int GetMarkingFilter()const{return PointMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
    bool GetStartPoint(double* pos);
    bool GetEndPoint(double* pos);
	void CopyFrom(const HeeksObj* object){operator=(*((HPoint*)object));}
	bool IsDifferent(HeeksObj* other);
	void Draw(wxDC& dc);
    void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
};
