// HILine.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "EndedObject.h"

class HILine: public EndedObject
{
private:

    // Private no-arg constructor
    HILine();

public:

    static const int ObjType = ILineType;


    PropertyVertex m_start;
    PropertyVertex m_end;
    PropertyLength m_length;

	HILine(const gp_Pnt &a, const gp_Pnt &b, const HeeksColor& col);
	HILine(const HILine &line);
    ~HILine(void);

	const HILine& operator=(const HILine &b);

    // HeeksObj's virtual functions
    void InitializeProperties();
    void OnPropertySet(Property& prop);
    void GetProperties(std::list<Property *> *list);
	int GetMarkingFilter()const{return ILineMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point);
	bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point);
	int Intersects(const HeeksObj *object, std::list< double > *rl)const;
	void CopyFrom(const HeeksObj* object){operator=(*((HILine*)object));}
 	bool GetEndPoint(double* pos);
	bool GetStartPoint(double* pos);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	gp_Lin GetLine()const;
};
