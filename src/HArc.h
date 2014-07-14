// HArc.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "EndedObject.h"

class HArc: public EndedObject{
private:

	PropertyVertex m_start;
	PropertyVertex m_end;
	PropertyVertex m_centre;
	PropertyVector m_axis_direction;
	PropertyLength m_length;

public:

	PropertyLength m_radius;
	gp_Ax1 m_axis;
	HPoint* C;

	~HArc(void);
	HArc(const gp_Pnt &a, const gp_Pnt &b, const gp_Circ &c, const HeeksColor& col);
	HArc(const HArc &arc);

	const HArc& operator=(const HArc &b);

	gp_Circ GetCircle() const;
	void SetCircle(gp_Circ c);
	bool IsIncluded(gp_Pnt pnt);

    void InitializeProperties();
    void OnPropertyEdit(Property& prop);
    void GetProperties(std::list<Property *> *list);

	// HeeksObj's virtual functions
	int GetType()const{return ArcType;}
	int GetMarkingFilter()const{return ArcMarkingFilter;}
	int GetIDGroupType()const{return LineType;}
	void glCommands(bool select, bool marked, bool no_color);
	void Draw(wxDC& dc);
	void GetBox(CBox &box);
	const wxChar* GetTypeString(void)const{return _("Arc");}
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point);
	bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point);
	bool Stretch(const double *p, const double* shift, void* data);
	void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true)const;
	bool GetCentrePoint(double* pos);
	void WriteXML(TiXmlNode *root);
	int Intersects(const HeeksObj *object, std::list< double > *rl)const;
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool DescendForUndo(){return false;}
	bool IsDifferent(HeeksObj* other);
	HeeksObj* MakeACopyWithID();
	void ReloadPointers();

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

	bool Intersects(const gp_Pnt &pnt)const;
	gp_Vec GetSegmentVector(double fraction)const;
	gp_Pnt GetPointAtFraction(double fraction)const;
	static bool TangentialArc(const gp_Pnt &p0, const gp_Vec &v0, const gp_Pnt &p1, gp_Pnt &centre, gp_Dir &axis);
	bool UsesID(){return true;}
	void Reverse();
	double IncludedAngle()const;
};
