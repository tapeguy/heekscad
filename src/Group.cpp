// Group.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Group.h"
#include "Shape.h"

CGroup::CGroup()
 : ObjList(ObjType), m_custom_grippers(false), m_custom_grippers_just_one_axis(true)
{
	InitializeProperties();
	m_title = _("Group");
	m_gripper_datum_set = false;
}

void CGroup::InitializeProperties()
{
	m_custom_grippers.Initialize( _("custom grippers"), this);
	m_custom_grippers_just_one_axis.Initialize( _("custom grippers just one axis"), this);
	m_o.Initialize(_("datum"), this);
	m_px.Initialize(_("x"), this);
	m_py.Initialize(_("y"), this);
	m_pz.Initialize(_("z"), this);
}

void CGroup::OnPropertySet(Property& prop)
{
	if(prop == m_custom_grippers_just_one_axis) {
		bool enabled = m_custom_grippers_just_one_axis.IsSet();
		m_o.SetVisible(enabled);
		m_px.SetVisible(enabled);
		m_py.SetVisible(enabled);
		m_pz.SetVisible(enabled);
	}
	else {
		ObjList::OnPropertySet(prop);
	}
}

void CGroup::WriteXML(TiXmlNode *root)
{
	TiXmlElement * element = new TiXmlElement( "Group" );
	root->LinkEndChild( element );
    element->SetAttribute("title", m_title.utf8_str());
	element->SetAttribute("custom_grippers", m_custom_grippers ? 1:0);
	element->SetAttribute("custom_grippers_one_axis", m_custom_grippers_just_one_axis ? 1:0);
	element->SetAttribute("gripper_datum_set", m_gripper_datum_set ? 1:0);
	if(m_gripper_datum_set)
	{
		element->SetDoubleAttribute("ox", m_o.X());
		element->SetDoubleAttribute("oy", m_o.Y());
		element->SetDoubleAttribute("oz", m_o.Z());
		element->SetDoubleAttribute("pxx", m_px.X());
		element->SetDoubleAttribute("pxy", m_px.Y());
		element->SetDoubleAttribute("pxz", m_px.Z());
		element->SetDoubleAttribute("pyx", m_py.X());
		element->SetDoubleAttribute("pyy", m_py.Y());
		element->SetDoubleAttribute("pyz", m_py.Z());
		element->SetDoubleAttribute("pzx", m_pz.X());
		element->SetDoubleAttribute("pzy", m_pz.Y());
		element->SetDoubleAttribute("pzz", m_pz.Z());
	}

	// instead of ObjList::WriteBaseXML(element), write the id of solids, or the object
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		if(CShape::IsTypeAShape(object->GetType()))
		{
			TiXmlElement* solid_element = new TiXmlElement( "solid" );
			element->LinkEndChild( solid_element );
			solid_element->SetAttribute("id", object->GetID());
		}

		object->WriteXML(element);
	}
	HeeksObj::WriteBaseXML(element);
}

// static member function
HeeksObj* CGroup::ReadFromXMLElement(TiXmlElement* element)
{
	CGroup* new_object = new CGroup;

	// instead of ( ObjList:: ) new_object->ReadBaseXML(pElem);

	if(element->Attribute("title"))new_object->m_title = Ctt(element->Attribute("title"));
	int int_for_bool;
	if(element->Attribute("custom_grippers", &int_for_bool))new_object->m_custom_grippers.SetValue(int_for_bool != 0);
	if(element->Attribute("custom_grippers_one_axis", &int_for_bool))new_object->m_custom_grippers_just_one_axis.SetValue(int_for_bool != 0);
	if(element->Attribute("gripper_datum_set", &int_for_bool))new_object->m_gripper_datum_set = (int_for_bool != 0);
	if(new_object->m_gripper_datum_set)
	{
		double o[3], px[3], py[3], pz[3];
		element->Attribute("ox", &o[0]);
		element->Attribute("oy", &o[1]);
		element->Attribute("oz", &o[2]);
		new_object->m_o = make_point(o);
		element->Attribute("pxx", &px[0]);
		element->Attribute("pxy", &px[1]);
		element->Attribute("pxz", &px[2]);
		new_object->m_px = make_point(px);
		element->Attribute("pyx", &py[0]);
		element->Attribute("pyy", &py[1]);
		element->Attribute("pyz", &py[2]);
		new_object->m_py = make_point(py);
		element->Attribute("pzx", &pz[0]);
		element->Attribute("pzy", &pz[1]);
		element->Attribute("pzz", &pz[2]);
		new_object->m_pz = make_point(pz);
	}

	// loop through all the objects
	for(TiXmlElement* pElem = TiXmlHandle(element).FirstChildElement().Element(); pElem; pElem = pElem->NextSiblingElement())
	{
		std::string name(pElem->Value());
		if(name == "solid")
		{
			int id = 0;
			pElem->Attribute("id", &id);
			new_object->m_loaded_solid_ids.push_back(id); // solid found after load with CGroup::MoveSolidsToGroupsById
		}
		else
		{
			// load other objects normal
			HeeksObj* object = wxGetApp().ReadXMLElement(pElem);
			if(object)new_object->Add(object);
		}
	}

	new_object->HeeksObj::ReadBaseXML(element);

	return (ObjList*)new_object;
}

// static
void CGroup::MoveSolidsToGroupsById(HeeksObj* object)
{
	std::list<HeeksObj*> objects;
	for(HeeksObj* o = object->GetFirstChild(); o; o = object->GetNextChild())
	{
		objects.push_back(o);
	}

	if(object->GetType() == GroupType)
	{
		CGroup* group = (CGroup*)object;

		for(std::list<int>::iterator It = group->m_loaded_solid_ids.begin(); It != group->m_loaded_solid_ids.end(); It++)
		{
			int id = *It;
			HeeksObj* o = wxGetApp().GetIDObject(SolidType, id);
			if (o != NULL)
			{
                o->GetOwner()->Remove(o);
				o->RemoveOwner();
                group->Add(o);
			}
		}
	}

	for(std::list<HeeksObj*>::iterator It = objects.begin(); It != objects.end(); It++)
	{
		HeeksObj* object = *It;
		MoveSolidsToGroupsById(object);
	}
}

const wxBitmap &CGroup::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/group.png")));
	return *icon;
}

bool CGroup::Stretch(const double *p, const double* shift, void* data){
	gp_Pnt vp = make_point(p);
	gp_Vec vshift = make_vector(shift);
	m_px = vp.XYZ() + vshift.XYZ();
	return false;
}

void CGroup::GetGripperPositions(std::list<GripData> *list, bool just_for_endof){
	if(m_custom_grippers)
	{
		if(!m_gripper_datum_set)
		{
			CBox box;
			GetBox(box);
			if(!box.m_valid)return;
			m_o = gp_Pnt((box.MinX() + box.MaxX())/2, (box.MinY() + box.MaxY())/2, (box.MinZ() + box.MaxZ())/2);
			m_px = gp_Pnt(box.MaxX(), (box.MinY() + box.MaxY())/2, (box.MinZ() + box.MaxZ())/2);
			m_py = gp_Pnt((box.MinX() + box.MaxX())/2, box.MaxY(), (box.MinZ() + box.MaxZ())/2);
			m_pz = gp_Pnt((box.MinX() + box.MaxX())/2, (box.MinY() + box.MaxY())/2, box.MaxZ());
			m_gripper_datum_set = true;
		}

		list->push_back(GripData(GripperTypeTranslate, m_o.X(), m_o.Y(), m_o.Z(), NULL));
		if(m_custom_grippers_just_one_axis)
		{
			list->push_back(GripData(GripperTypeStretch, m_px.X(), m_px.Y(), m_px.Z(), NULL));
			list->push_back(GripData(GripperTypeRotateObjectYZ, m_py.X(), m_py.Y(), m_py.Z(), NULL));
			list->push_back(GripData(GripperTypeRotateObjectYZ, m_pz.X(), m_pz.Y(), m_pz.Z(), NULL));
		}
		else
		{
			list->push_back(GripData(GripperTypeRotateObjectXY, m_px.X(), m_px.Y(), m_px.Z(), NULL));
			list->push_back(GripData(GripperTypeRotateObjectYZ, m_py.X(), m_py.Y(), m_py.Z(), NULL));
			list->push_back(GripData(GripperTypeRotateObjectXZ, m_pz.X(), m_pz.Y(), m_pz.Z(), NULL));
		}
	}
	else
	{
		ObjList::GetGripperPositions(list, just_for_endof);
	}
}

static CGroup* object_for_tools = NULL;
static PropertyVertex* vertex_for_pick_pos = NULL;
static void(*callback_for_pick_pos)(const double*) = NULL;

static void on_set_o(const double* pos)
{
	gp_Vec shift(object_for_tools->m_o, make_point(pos));
	object_for_tools->m_o = make_point(pos);
	object_for_tools->m_px = object_for_tools->m_px.XYZ() + shift.XYZ();
	object_for_tools->m_py = object_for_tools->m_py.XYZ() + shift.XYZ();
	object_for_tools->m_pz = object_for_tools->m_pz.XYZ() + shift.XYZ();
	wxGetApp().Repaint();
}

static void on_set_pos(const double* pos)
{
	vertex_for_pick_pos->SetValue(make_point(pos));
	wxGetApp().Repaint();
}

class PickPos: public Tool{
	// Tool's virtual functions
	void Run(){
		callback_for_pick_pos = on_set_pos;
		SetVertexPtr();
		double p[3];
		extract(*vertex_for_pick_pos, p);
		wxGetApp().PickPosition(GetTitle(), p, callback_for_pick_pos);
	}
	wxString BitmapPath(){ return wxGetApp().GetResFolder() + _T("/bitmaps/pickpos.png");}
	virtual void SetVertexPtr()=0;
};

class PickO: public PickPos{
	const wxChar* GetTitle(){return _("Move datum");}
	void SetVertexPtr(){
		vertex_for_pick_pos = &object_for_tools->m_o;
		callback_for_pick_pos = on_set_o;
	}
};

class PickPx: public PickPos{
	const wxChar* GetTitle(){return _("Move px");}
	void SetVertexPtr(){vertex_for_pick_pos = &object_for_tools->m_px;}
};

class PickPy: public PickPos{
	const wxChar* GetTitle(){return _("Move py");}
	void SetVertexPtr(){vertex_for_pick_pos = &object_for_tools->m_py;}
};

class PickPz: public PickPos{
	const wxChar* GetTitle(){return _("Move pz");}
	void SetVertexPtr(){vertex_for_pick_pos = &object_for_tools->m_pz;}
};

static PickO pick_o;
static PickPx pick_px;
static PickPy pick_py;
static PickPz pick_pz;

void CGroup::GetTools(std::list<Tool*>* t_list, const wxPoint* p)
{
	ObjList::GetTools(t_list, p);

	object_for_tools = this;
	if(m_custom_grippers)
	{
		t_list->push_back(&pick_o);
		t_list->push_back(&pick_px);
		t_list->push_back(&pick_py);
		t_list->push_back(&pick_pz);
	}
}

gp_Trsf CGroup::GetMatrix()
{
	return make_matrix(m_o, gp_Vec(m_o, m_px), gp_Vec(m_o, m_py));
}

bool CGroup::GetScaleAboutMatrix(double *m)
{
	if(m_custom_grippers)
	{
		gp_Trsf mat = GetMatrix();
		extract(mat, m);
		return true;
	}

	return ObjList::GetScaleAboutMatrix(m);
}

void CGroup::ModifyByMatrix(const double *m)
{
	ObjList::ModifyByMatrix(m);

	if(m_gripper_datum_set)
	{
		gp_Trsf mat = make_matrix(m);
		m_o.Transform(mat);
		m_px.Transform(mat);
		m_py.Transform(mat);
		m_pz.Transform(mat);
	}
}
