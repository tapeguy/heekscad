// HArea.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/IdNamedObj.h"
#include "../libarea/Area.h"

class HArea: public IdNamedObj
{
	static void WriteVertex(const CVertex& vertex, TiXmlNode *root);
	static void WriteCurve(const CCurve& curve, TiXmlNode *root);
	static void WriteArea(const CArea& area, TiXmlNode *root);
	static void ReadVertex(CVertex& vertex, TiXmlElement *root);
	static void ReadCurve(CCurve& curve, TiXmlElement *root);
	static void ReadArea(CArea& area, TiXmlElement *root);

private:
	PropertyInt m_number_curves;

public:

	static const int ObjType = AreaType;


	CArea m_area;
	CCurve* m_selected_curve;

	HArea(const CArea &a);
	HArea(const HArea &area);

    ~HArea(void);

	const HArea& operator=(const HArea &b);

    // HeeksObj's virtual functions
    void InitializeProperties();
    void GetProperties(std::list<Property *> *list);
	int GetMarkingFilter()const{return AreaMarkingFilter;}
	int GetIDGroupType()const{return SketchType;}
	void glCommands(bool select, bool marked, bool no_color);
	void Draw(wxDC& dc);
	void GetBox(CBox &box);
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void ModifyByMatrix(const double *mat);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	bool Stretch(const double *p, const double* shift, void* data);
	void WriteXML(TiXmlNode *root);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool DescendForUndo(){return false;}
	bool IsDifferent(HeeksObj* other);
	void SetClickMarkPoint(MarkedObject* marked_object, const double* ray_start, const double* ray_direction);
	bool UsesCustomSubNames(){return true;}

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
};
