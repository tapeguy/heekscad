// Pocket.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "Pocket.h"
#include "Shape.h"
#include "RuledSurface.h"
#include "Part.h"

HPocket::HPocket(double length) :
m_length(_("height"), _("Height"), this)
{
    m_length.SetValue ( length );
	m_faces->SetVisible ( false );
}

HPocket::HPocket() :
        m_length(_("height"), _("Height"), this)
{
}

bool HPocket::IsDifferent(HeeksObj* other)
{
	HPocket* pocket = (HPocket*)other;
	if(pocket->m_length != m_length)
		return true;

	return HeeksObj::IsDifferent(other);
}

void HPocket::ReloadPointers()
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

gp_Trsf HPocket::GetTransform()
{
	if(m_sketch && m_sketch->m_coordinate_system)
		return m_sketch->m_coordinate_system->GetMatrix();
	return gp_Trsf();
}

void HPocket::Update()
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

void HPocket::glCommands(bool select, bool marked, bool no_color)
{
	//TODO: only do this when the sketch is dirty

	glPushMatrix();
	if(m_sketch)
	{
		Update();
		if(m_sketch->m_coordinate_system)
			m_sketch->m_coordinate_system->ApplyMatrix();
//		DrawShapes();
	}

	//Draw everything else
	ObjList::glCommands(select,marked,no_color);
	glPopMatrix();

}

void HPocket::WriteXML(TiXmlNode *root)
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
HeeksObj* HPocket::ReadFromXMLElement(TiXmlElement* element)
{
	HPocket* new_object = new HPocket;
	return new_object;
}

// static
void HPocket::PocketSketch(CSketch* sketch, double length)
{
	HPocket *pad = new HPocket(length);
	sketch->Owner()->Add(pad,NULL);

	sketch->Owner()->Remove(sketch);
	sketch->RemoveOwner();

	sketch->m_draw_with_transform = false;
	pad->Add(sketch,NULL);
	pad->ReloadPointers();
}
