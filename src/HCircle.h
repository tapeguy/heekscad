// HCircle.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#ifdef MULTIPLE_OWNERS
#include "../interface/ObjList.h"
class HPoint;
class HCircle: public ObjList{
#else
#include "HPoint.h"
class HCircle: public HeeksObj{
#endif

private:
        PropertyVertex m_centre;
        PropertyVector m_axis_direction;

public:
	PropertyLength m_radius;
	gp_Ax1 m_axis;
	HPoint *C;

	~HCircle(void);
	HCircle(const gp_Circ &c, const HeeksColor& col);
	HCircle(const HCircle &c);

	const HCircle& operator=(const HCircle &c);

	// HeeksObj's virtual functions
	void InitializeProperties();
	int GetType()const{return CircleType;}
	int GetMarkingFilter()const{return CircleMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	const wxChar* GetTypeString(void)const{return _("Circle");}
	int GetIDGroupType()const{return SketchType;}
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void OnPropertyEdit(Property *prop);
	void GetProperties(std::list<Property *> *list);
	bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point);
	bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point);
	bool Stretch(const double *p, const double* shift, void* data);
	void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true)const;
	bool GetCentrePoint(double* pos);
	void WriteXML(TiXmlNode *root);
	int Intersects(const HeeksObj *object, std::list< double > *rl)const;
	bool DescendForUndo(){return false;}
	bool IsDifferent(HeeksObj* other);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	static bool GetLineTangentPoints(const gp_Circ& c1, const gp_Circ& c2, const gp_Pnt& a, const gp_Pnt& b, gp_Pnt& p1, gp_Pnt& p2);
	static bool GetLineTangentPoint(const gp_Circ& c, const gp_Pnt& a, const gp_Pnt& b, gp_Pnt& p);
	static bool GetArcTangentPoints(const gp_Circ& c, const gp_Lin &line, const gp_Pnt& p, double radius, gp_Pnt& p1, gp_Pnt& p2, gp_Pnt& centre, gp_Dir& axis);
	static bool GetArcTangentPoints(const gp_Circ& c1, const gp_Circ &c2, const gp_Pnt& a, const gp_Pnt& b, double radius, gp_Pnt& p1, gp_Pnt& p2, gp_Pnt& centre, gp_Dir& axis);
	static bool GetArcTangentPoints(const gp_Lin& l1, const gp_Lin &l2, const gp_Pnt& a, const gp_Pnt& b, double radius, gp_Pnt& p1, gp_Pnt& p2, gp_Pnt& centre, gp_Dir& axis);
	static bool GetArcTangentPoint(const gp_Lin& l, const gp_Pnt& a, const gp_Pnt& b, const gp_Vec *final_direction, double* radius, gp_Pnt& p, gp_Pnt& centre, gp_Dir& axis);
	static bool GetArcTangentPoint(const gp_Circ& c, const gp_Pnt& a, const gp_Pnt& b, const gp_Vec *final_direction, double* radius, gp_Pnt& p, gp_Pnt& centre, gp_Dir& axis);

	void SetCircle(gp_Circ c);
	gp_Circ GetCircle() const;
	void LoadFromDoubles();
	void LoadToDoubles();
};
