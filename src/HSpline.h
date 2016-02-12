// HSpline.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "EndedObject.h"
#include "../interface/PropertyList.h"

// CTangentialArc is used to calculate an arc given desired start ( p0 ), end ( p1 ) and start direction ( v0 )
class CTangentialArc
{
private:
    HeeksObj * m_owner;

public:
	gp_Pnt m_p0; // start point
	gp_Vec m_v0; // start direction
	gp_Pnt m_p1; // end point
	gp_Pnt m_c; // centre point
	gp_Dir m_a; // axis
	bool m_is_a_line;
	CTangentialArc(HeeksObj * owner, const gp_Pnt &p0, const gp_Vec &v0, const gp_Pnt &p1);
	bool radius_equal(const gp_Pnt &p, double tolerance) const;
	double radius() const;
	HeeksObj* MakeHArc() const;
};

class HSpline : public EndedObject
{
private:

    bool spline_created;
	PropertyInt m_degree;
	PropertyCheck m_rational;
	PropertyCheck m_periodic;
	PropertyList m_knots;
	PropertyList m_poles;

	// Private no-arg constructor
	HSpline();

public:

	static const int ObjType = SplineType;


    Handle(Geom_BSplineCurve) m_spline;

	HSpline(const Geom_BSplineCurve &s, const HeeksColor& col);
	HSpline(const Handle_Geom_BSplineCurve s, const HeeksColor& col);
	HSpline(const std::list<gp_Pnt> &points, const HeeksColor& col);
	HSpline(const HSpline &c);

    ~HSpline(void);

	const HSpline& operator=(const HSpline &c);

    // HeeksObj's virtual functions
    void InitializeProperties();
    void OnPropertySet(Property& prop);
    void GetProperties(std::list<Property *> *list);
	int GetMarkingFilter()const{return ArcMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	void RedistributePoles();
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point);
	bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point);
	bool Stretch(const double *p, const double* shift, void* data);
	void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true) const;
	int Intersects(const HeeksObj *object, std::list< double > *rl) const;
	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	bool IsDifferent(HeeksObj* other);
	bool GetStartPoint(double* pos);
	bool GetEndPoint(double* pos);

	void ToBiarcs(std::list<HeeksObj*> &new_spans, double tolerance);
	static void ToBiarcs(HeeksObj * owner, const Handle_Geom_BSplineCurve s, std::list<HeeksObj*> &new_spans, double tolerance, double first_parameter, double last_parameter);
	void Reverse();

	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	wxString ToString() const;

protected:
	static void ReadKnotsXML(Property * prop, TiXmlElement *element);
    static void ReadPolesXML(Property * prop, TiXmlElement *element);
};
