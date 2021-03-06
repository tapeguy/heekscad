// HLine.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "EndedObject.h"

class HLine: public EndedObject
{
public:

    static const int ObjType = LineType;


    PropertyVertex m_start;
	PropertyVertex m_end;
	PropertyLength m_length;

	HLine();
	HLine(const gp_Pnt &a, const gp_Pnt &b, const HeeksColor& col);
	HLine(const HLine &line);
	~HLine(void);

	const HLine& operator=(const HLine &b);

    // HeeksObj's virtual functions
    void InitializeProperties();
    void OnPropertySet(Property& prop);
    void GetProperties(std::list<Property *> *list);
	int GetMarkingFilter()const{return LineMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void Draw(wxDC& dc);
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	bool GetMidPoint(double* pos);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point);
	bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point);
	void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true)const;
	int Intersects(const HeeksObj *object, std::list< double > *rl)const;
	void CopyFrom(const HeeksObj* object){operator=(*((HLine*)object));}
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	gp_Lin GetLine()const;
	bool Intersects(const gp_Pnt &pnt)const;
	gp_Vec GetSegmentVector(double fraction);
	void Reverse();
};
