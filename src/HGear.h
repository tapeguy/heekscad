// HGear.h
// Copyright (c) 2011, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/IdNamedObj.h"

class HGear: public IdNamedObj
{
	void SetSegmentsVariables(void(*callbackfunc)(const double *p))const;

public:

	static const int ObjType = GearType;


	PropertyCoord m_pos; // coordinate system defining position and orientation
	PropertyInt m_num_teeth;
	PropertyDouble m_module;
	PropertyDouble m_addendum_offset;
	PropertyDouble m_addendum_multiplier;
	PropertyDouble m_dedendum_multiplier;
	PropertyDouble m_pressure_angle;
	PropertyDouble m_tip_relief;
	PropertyDouble m_depth;
	PropertyDouble m_cone_half_angle;  // 0 for a cylinder ( spur gear ), 90 for a circular rack
	PropertyDouble m_angle;            // draw the gear rotated anti-clockwise by this angle

	HGear();
	HGear(const HGear &o);
	~HGear(void);

	const HGear& operator=(const HGear &o);

	// HeeksObj's virtual functions
	void InitializeProperties();
	int GetMarkingFilter()const{return GearMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool GetScaleAboutMatrix(double *m);
	bool DescendForUndo(){return false;}
	bool IsDifferent(HeeksObj* other);
	void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true)const;
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	void GetOneToothSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true)const;
	HeeksObj* MakeSketch()const;
	double GetClearanceMM()const;
};
