// CoordinateSystem.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"

class CoordinateSystem: public HeeksObj
{
private:
	PropertyDouble m_vert_angle;
	PropertyDouble m_horiz_angle;
	PropertyDouble m_twist_angle;

public:
	PropertyVertex m_o;
	PropertyVector m_x;
	PropertyVector m_y;

	wxString m_title;

	static PropertyDouble size;
	static PropertyCheck size_is_pixels; // false for mm
	static bool rendering_current;

	CoordinateSystem(const wxString& str, const gp_Pnt &o, const gp_Dir &x, const gp_Dir &y);
	CoordinateSystem(const CoordinateSystem &c);
	~CoordinateSystem(void);

	const CoordinateSystem& operator=(const CoordinateSystem &c);

    void InitializeProperties();
    void OnPropertyEdit(Property& prop);

	// HeeksObj's virtual functions
	int GetType()const{return CoordinateSystemType;}
	int GetMarkingFilter()const{return CoordinateSystemMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	const wxChar* GetTypeString(void)const{return _("Coordinate System");}
	const wxChar* GetShortString(void)const{return m_title.c_str();}
	bool CanEditString(void)const{return true;}
	void OnEditString(const wxChar* str);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool GetScaleAboutMatrix(double *m);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	void WriteXML(TiXmlNode *root);

	gp_Trsf GetMatrix();
	void ApplyMatrix();

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
	static void RenderArrow();
	static void RenderDatum(bool bright, bool solid); // render a coordinate system at 0, 0, 0
	static void AxesToAngles(const gp_Dir &x, const gp_Dir &y, double &v_angle, double &h_angle, double &t_angle);
	static void AnglesToAxes(const double &v_angle, const double &h_angle, const double &t_angle, gp_Dir &x, gp_Dir &y);
	void PickFrom3Points();
	void PickFrom1Point();
};
