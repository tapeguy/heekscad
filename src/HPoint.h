// HPoint.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"

class HPoint: public HeeksObj{
protected:

    PropertyColor m_color;

public:

	PropertyVertex m_p;
	bool m_draw_unselected;
	double mx,my,mz;

	~HPoint(void);
	HPoint(const gp_Pnt &p, const HeeksColor& col);
	HPoint(const HPoint &p);

	const HPoint& operator=(const HPoint &b);

    void SetColor(const HeeksColor &col) { m_color = col; }
    const HeeksColor& GetColor() const { return m_color; }

	// HeeksObj's virtual functions
	int GetType()const{return PointType;}
	int GetMarkingFilter()const{return PointMarkingFilter;}
	void glCommands(bool select, bool marked, bool no_color);
	void GetBox(CBox &box);
	const wxChar* GetTypeString(void)const{return _("Point");}
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool GetStartPoint(double* pos);
	bool GetEndPoint(double* pos);
	void CopyFrom(const HeeksObj* object){operator=(*((HPoint*)object));}
	void WriteXML(TiXmlNode *root);
	void LoadFromDoubles();
	void LoadToDoubles();
	bool IsDifferent(HeeksObj* other);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);

	void Draw(wxDC& dc);


	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
};
