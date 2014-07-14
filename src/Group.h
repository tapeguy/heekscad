// Group.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/ObjList.h"

class CGroup: public ObjList{
	std::list<int> m_loaded_solid_ids; // only used during loading xml file

public:
	wxString m_title;
	bool m_gripper_datum_set;
	PropertyCheck m_custom_grippers;
	PropertyCheck m_custom_grippers_just_one_axis;
	PropertyVertex m_o;
	PropertyVertex m_px;
	PropertyVertex m_py;
	PropertyVertex m_pz;

	CGroup();
	void InitializeProperties();
	void OnPropertyEdit(Property& prop);
	const wxChar* GetTypeString(void)const{return _("Group");}
	int GetType()const{return GroupType;}
	HeeksObj *MakeACopy(void)const{ return new CGroup(*this);}
	const wxBitmap &GetIcon();
	void WriteXML(TiXmlNode *root);
	bool UsesID(){return true;}
	const wxChar* GetShortString(void)const{return m_title.c_str();}
	bool CanEditString(void)const{return true;}
	void OnEditString(const wxChar* str);
	bool Stretch(const double *p, const double* shift, void* data);
	void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
	void GetTools(std::list<Tool*>* t_list, const wxPoint* p);
	bool GetScaleAboutMatrix(double *m);
	void ModifyByMatrix(const double *m);

	static void MoveSolidsToGroupsById(HeeksObj* object);
	gp_Trsf GetMatrix();

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);
};

