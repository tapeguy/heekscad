// HArc.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "EndedObject.h"

class HArc: public EndedObject {
private:

    bool in_edit;
    PropertyLength m_radius;
    PropertyVector m_axis_direction;
    PropertyLength m_length;

public:

    static const int ObjType = ArcType;

    HPoint* C;

    HArc();
    HArc(const gp_Pnt &a, const gp_Pnt &b, const gp_Circ &c, const HeeksColor& col);
    HArc(const HArc &arc);
    ~HArc();

    const HArc& operator=(const HArc &b);

    gp_Circ GetCircle() const;
    void SetCircle(gp_Circ c);
    bool IsIncluded(gp_Pnt pnt);

    // HeeksObj's virtual functions
    void InitializeProperties();
    void GetProperties(std::list<Property *> *list);
    void OnChildModified(HeeksObj * child, Property& prop);
	int GetMarkingFilter()const{return ArcMarkingFilter;}
	int GetIDGroupType()const{return LineType;}
	void glCommands(bool select, bool marked, bool no_color);
	void Draw(wxDC& dc);
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void) const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point);
	bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point);
	bool Stretch(const double *p, const double* shift, void* data);
	void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true) const;
	bool GetCentrePoint(double* pos);
	int Intersects(const HeeksObj *object, std::list< double > *rl) const;
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool DescendForUndo(){return false;}
	bool IsDifferent(HeeksObj* other);
	void ReloadPointers();

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	bool Intersects(const gp_Pnt &pnt) const;
	gp_Vec GetSegmentVector(double fraction) const;
	gp_Pnt GetPointAtFraction(double fraction) const;
	static bool TangentialArc(const gp_Pnt &p0, const gp_Vec &v0, const gp_Pnt &p1, gp_Pnt &centre, gp_Dir &axis);
	void Reverse();
	double IncludedAngle() const;
	double Radius() const;
	gp_Vec Direction() const { return m_axis_direction; }
};
