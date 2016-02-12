// HEllipse.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "HPoint.h"
#include "../interface/IdNamedObjList.h"
#include "../interface/HeeksColor.h"

class HEllipse: public IdNamedObjList
{
public:

    static const int ObjType = EllipseType;

    HPoint* C;
	PropertyVertex m_centre;
	PropertyVector m_axis;
	PropertyLength m_majr;
	PropertyLength m_minr;
	PropertyDouble m_rot;
	PropertyDouble m_start;
	PropertyDouble m_end;
	PropertyVector m_zdir;
	PropertyVector m_xdir;

	HEllipse();
	HEllipse(const gp_Elips &c, const HeeksColor& col);
	HEllipse(const gp_Elips &c, double start, double end, const HeeksColor& col);
	HEllipse(const HEllipse &c);
	~HEllipse(void);

	const HEllipse& operator=(const HEllipse &c);

	void SetRotation(double rot);
	void SetEllipse(gp_Elips e);
	gp_Elips GetEllipse() const;

    // HeeksObj's virtual functions
	virtual bool UsesColor() { return true; }
    void InitializeProperties();
    void OnPropertySet(Property& prop);
    void GetProperties(std::list<Property *> *list);
	int GetMarkingFilter()const{return CircleMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point);
	bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point);
	bool Stretch(const double *p, const double* shift, void* data);
	void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true)const;
	bool GetCentrePoint(double* pos);
	int Intersects(const HeeksObj *object, std::list< double > *rl)const;
	void ReloadPointers();
	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
};

