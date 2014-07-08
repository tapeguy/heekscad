// Cylinder.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Cylinder.h"
#include "Gripper.h"
#include "MarkedList.h"


static TopoDS_Solid MakeCylinder(const gp_Ax2& pos, double diameter, double height)
{
	gp_Ax2 pos2 = pos;
	if(height<0)
	{
		pos2 = gp_Ax2(pos.Location(), -(pos.Direction()));
		height = fabs(height);
	}

	return BRepPrimAPI_MakeCylinder(pos2, diameter/2, height);
}

CCylinder::CCylinder(const gp_Ax2& pos, double diameter, double height, const wxChar* title, const HeeksColor& col, float opacity)
 : CSolid(MakeCylinder(pos, diameter, height), title, col, opacity),
 m_pos(pos), m_diameter(diameter), m_height(height)
{
	InitializeProperties();
}

CCylinder::CCylinder(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity)
 : CSolid(solid, title, col, opacity),
 m_pos(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0)), m_diameter(0.0), m_height(0.0)
{
	InitializeProperties();
}

void CCylinder::InitializeProperties()
{
	m_diameter.Initialize(_("diameter"), this);
	m_height.Initialize(_("height"), this);
}

const wxBitmap &CCylinder::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/cyl.png")));
	return *icon;
}

HeeksObj *CCylinder::MakeACopy(void)const
{
	return new CCylinder(*this);
}

bool CCylinder::IsDifferent(HeeksObj* other)
{
	CCylinder* cyl = (CCylinder*)other;
	if(cyl->m_diameter != m_diameter || cyl->m_height != m_height)
		return true;

	if(!IsEqual(cyl->m_pos,m_pos))
		return true;
	
	return CShape::IsDifferent(other);
}

void CCylinder::MakeTransformedShape(const gp_Trsf &mat)
{
	m_pos.Transform(mat);
	double scale = gp_Vec(1, 0, 0).Transformed(mat).Magnitude();
	m_diameter = fabs(m_diameter * scale);
	m_height = fabs(m_height * scale);
	m_shape = MakeCylinder(m_pos, m_diameter, m_height);
}

wxString CCylinder::StretchedName(){ return _("Stretched Cylinder");}

void CCylinder::GetProperties(std::list<Property *> *list)
{
//      CoordinateSystem::GetAx2Properties(list, m_pos);
	CSolid::GetProperties(list);
}

void CCylinder::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	gp_Pnt o = m_pos.Location();
	gp_Pnt px(o.XYZ() + m_pos.XDirection().XYZ() * (m_diameter/2));
	gp_Dir z_dir = m_pos.XDirection() ^ m_pos.YDirection();
	gp_Pnt pyz(o.XYZ() + m_pos.YDirection().XYZ() * (m_diameter/2) + z_dir.XYZ() * m_height);
	gp_Pnt pmxz(o.XYZ() + m_pos.XDirection().XYZ() * (-m_diameter/2) + z_dir.XYZ() * m_height);
	gp_Pnt pz(o.XYZ() + z_dir.XYZ() * m_height);
	list->push_back(GripData(GripperTypeTranslate,o.X(),o.Y(),o.Z(),NULL));
	list->push_back(GripData(GripperTypeObjectScaleXY,px.X(),px.Y(),px.Z(),NULL));
	list->push_back(GripData(GripperTypeRotateObject,pyz.X(),pyz.Y(),pyz.Z(),NULL));
	list->push_back(GripData(GripperTypeRotateObject,pmxz.X(),pmxz.Y(),pmxz.Z(),NULL));
	list->push_back(GripData(GripperTypeObjectScaleZ,pz.X(),pz.Y(),pz.Z(),NULL));
}

void CCylinder::OnPropertyEdit(Property* prop)
{
	if (prop == &m_diameter || prop == &m_height) {
		CCylinder* new_object = new CCylinder(m_pos, m_diameter, m_height, m_title.c_str(), m_color, m_opacity);
		new_object->CopyIDsFrom(this);
		Owner()->Add(new_object, NULL);
		Owner()->Remove(this);
		if(wxGetApp().m_marked_list->ObjectMarked(this))
		{
			wxGetApp().m_marked_list->Remove(this,false);
			wxGetApp().m_marked_list->Add(new_object, true);
		}
	}
	else {
		CSolid::OnPropertyEdit(prop);
	}
}

int CCylinder::GetCentrePoints(double* pos, double* pos2)
{
	gp_Pnt o = m_pos.Location();
	gp_Dir z_dir = m_pos.XDirection() ^ m_pos.YDirection();
	gp_Pnt pz(o.XYZ() + z_dir.XYZ() * m_height);
	extract(o, pos);
	extract(pz, pos2);
	return 2;
}

bool CCylinder::GetScaleAboutMatrix(double *m)
{
	gp_Trsf mat = make_matrix(m_pos.Location(), m_pos.XDirection(), m_pos.YDirection());
	extract(mat, m);
	return true;
}

bool CCylinder::Stretch(const double *p, const double* shift, void* data)
{
	gp_Pnt vp = make_point(p);
	gp_Vec vshift = make_vector(shift);

	gp_Pnt o = m_pos.Location();
	gp_Pnt px(o.XYZ() + m_pos.XDirection().XYZ() * (m_diameter/2));
	gp_Dir z_dir = m_pos.XDirection() ^ m_pos.YDirection();
	gp_Pnt pz(o.XYZ() + z_dir.XYZ() * m_height);

	bool make_a_new_cylinder = false;

	if(px.IsEqual(vp, wxGetApp().m_geom_tol)){
		px = px.XYZ() + vshift.XYZ();
		double new_x = gp_Vec(px.XYZ()) * gp_Vec(m_pos.XDirection()) - gp_Vec(o.XYZ()) * gp_Vec(m_pos.XDirection());
		double new_y = gp_Vec(px.XYZ()) * gp_Vec(m_pos.YDirection()) - gp_Vec(o.XYZ()) * gp_Vec(m_pos.YDirection());
		make_a_new_cylinder = true;
		m_diameter = sqrt(new_x * new_x + new_y * new_y) * 2;
	}
	else if(pz.IsEqual(vp, wxGetApp().m_geom_tol)){
		pz = pz.XYZ() + vshift.XYZ();
		double new_height = gp_Vec(pz.XYZ()) * gp_Vec(z_dir) - gp_Vec(o.XYZ()) * gp_Vec(z_dir);
		if(new_height > 0){
			make_a_new_cylinder = true;
			m_height = new_height;
		}
	}

	if(make_a_new_cylinder)
	{
		CCylinder* new_object = new CCylinder(m_pos, m_diameter, m_height, m_title.c_str(), m_color, m_opacity);
		new_object->CopyIDsFrom(this);
		Owner()->Add(new_object, NULL);
		Owner()->Remove(this);
		wxGetApp().m_marked_list->Clear(true);
		wxGetApp().m_marked_list->Add(new_object, true);
	}

	return true;
}

void CCylinder::SetXMLElement(TiXmlElement* element)
{
	const gp_Pnt& l = m_pos.Location();
	element->SetDoubleAttribute("lx", l.X());
	element->SetDoubleAttribute("ly", l.Y());
	element->SetDoubleAttribute("lz", l.Z());

	const gp_Dir& d = m_pos.Direction();
	element->SetDoubleAttribute("dx", d.X());
	element->SetDoubleAttribute("dy", d.Y());
	element->SetDoubleAttribute("dz", d.Z());

	const gp_Dir& x = m_pos.XDirection();
	element->SetDoubleAttribute("xx", x.X());
	element->SetDoubleAttribute("xy", x.Y());
	element->SetDoubleAttribute("xz", x.Z());

	element->SetDoubleAttribute("d", m_diameter);
	element->SetDoubleAttribute("h", m_height);

	CSolid::SetXMLElement(element);
}

void CCylinder::SetFromXMLElement(TiXmlElement* pElem)
{
	// get the attributes
	double l[3] = {0, 0, 0};
	double d[3] = {0, 0, 1};
	double x[3] = {1, 0, 0};

	for(TiXmlAttribute* a = pElem->FirstAttribute(); a; a = a->Next())
	{
		std::string name(a->Name());
		if(name == "lx")	 {l[0] = a->DoubleValue();}
		else if(name == "ly"){l[1] = a->DoubleValue();}
		else if(name == "lz"){l[2] = a->DoubleValue();}

		else if(name == "dx"){d[0] = a->DoubleValue();}
		else if(name == "dy"){d[1] = a->DoubleValue();}
		else if(name == "dz"){d[2] = a->DoubleValue();}

		else if(name == "xx"){x[0] = a->DoubleValue();}
		else if(name == "xy"){x[1] = a->DoubleValue();}
		else if(name == "xz"){x[2] = a->DoubleValue();}

		else if(name == "d"){m_diameter = a->DoubleValue();}
		else if(name == "h"){m_height = a->DoubleValue();}
	}

	m_pos = gp_Ax2(make_point(l), make_vector(d), make_vector(x));

	CSolid::SetFromXMLElement(pElem);
}
