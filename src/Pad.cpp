// Pad.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Pad.h"
#include "Shape.h"
#include "RuledSurface.h"
#include "Part.h"

CPad::CPad(double length) :
m_length(_("height"), _("Height"), this)
{
    m_length.SetValue(length);
}

CPad::CPad() :
m_length(_("height"), _("Height"), this)
{
}

bool CPad::IsDifferent(HeeksObj* other)
{
	CPad* pad = (CPad*)other;
	if(pad->m_length != m_length)
		return true;

	return HeeksObj::IsDifferent(other);
}

void CPad::ReloadPointers()
{
	DynamicSolid::ReloadPointers();

	HeeksObj *child = GetFirstChild();
	while(child)
	{
		CSketch* sketch = dynamic_cast<CSketch*>(child);
		if(sketch)
		{
			m_sketch = sketch;
			break;
		}
		child = GetNextChild();
	}

	Update();
}

void CPad::Update()
{
	if(m_sketch)
	{
		std::vector<TopoDS_Face> faces = m_sketch->GetFaces();
		std::list<TopoDS_Shape> facelist(faces.begin(),faces.end());
		std::list<TopoDS_Shape> new_shapes;
		CreateExtrusions(facelist, new_shapes, gp_Vec(0, 0, m_length), 0.0, true);

		SetShapes(new_shapes);
	}

	DynamicSolid* solid = dynamic_cast<DynamicSolid*>(Owner());
	if(solid)
		solid->Update();
}

void CPad::glCommands(bool select, bool marked, bool no_color)
{
	//TODO: only do this when the sketch is dirty

	glPushMatrix();
	if(m_sketch)
	{
		Update();
		if(m_sketch->m_coordinate_system)
			m_sketch->m_coordinate_system->ApplyMatrix();
	//	DrawShapes();
	}

	//Draw everything else
	ObjList::glCommands(select,marked,no_color);
	glPopMatrix();

}

gp_Trsf CPad::GetTransform()
{
	if(m_sketch && m_sketch->m_coordinate_system)
		return m_sketch->m_coordinate_system->GetMatrix();
	return gp_Trsf();
}

void CPad::WriteXML(TiXmlNode *root)
{
	TiXmlElement * element = new TiXmlElement( "Pad" );
	return;
	root->LinkEndChild( element );

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
HeeksObj* CPad::ReadFromXMLElement(TiXmlElement* element)
{
	CPad* new_object = new CPad;
	return new_object;
}

// static
void CPad::PadSketch(CSketch* sketch, double length)
{
	CPad *pad = new CPad(length);
	sketch->Owner()->Add(pad,NULL);

	sketch->Owner()->Remove(sketch);
	sketch->RemoveOwner();

	sketch->m_draw_with_transform = false;
	pad->Add(sketch,NULL);
	pad->ReloadPointers();
}
