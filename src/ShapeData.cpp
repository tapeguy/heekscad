// ShapeData.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "ShapeData.h"
#include "Solid.h"

CShapeData::CShapeData(): m_xml_element("")
{
	m_id = -1;
	m_solid_type = SOLID_TYPE_UNKNOWN;
	m_visible = true;
}

CShapeData::CShapeData(CShape* shape): m_xml_element("")
{
	m_id = shape->GetID();
	m_title = shape->GetTitle();
	m_visible = shape->IsVisible();
	m_solid_type = SOLID_TYPE_UNKNOWN;
	if(shape->GetType() == SolidType)m_solid_type = ((CSolid*)shape)->GetSolidType();
	shape->SetXMLElement(&m_xml_element);

	for(HeeksObj* object = shape->m_faces->GetFirstChild(); object; object = shape->m_faces->GetNextChild())
	{
		m_face_ids.push_back(object->GetID());
	}

	for(HeeksObj* object = shape->m_edges->GetFirstChild(); object; object = shape->m_edges->GetNextChild())
	{
		m_edge_ids.push_back(object->GetID());
	}

	for(HeeksObj* object = shape->m_vertices->GetFirstChild(); object; object = shape->m_vertices->GetNextChild())
	{
		m_vertex_ids.push_back(object->GetID());
	}

}

void CShapeData::SetShape(CShape* shape, bool apply_id)
{
    if(m_title.length() > 0) {
        shape->SetTitle(m_title);
    }
    else {
        shape->m_title_made_from_id = true;
    }

    if(apply_id && (m_id != -1)) {
        shape->SetID(m_id);
    }

	shape->SetVisible ( m_visible );
	shape->SetFromXMLElement(&m_xml_element);

	{
		std::list<int>::iterator It = m_face_ids.begin();
		for(HeeksObj* object = shape->m_faces->GetFirstChild(); object && It != m_face_ids.end(); object = shape->m_faces->GetNextChild(), It++)
		{
			object->SetID(*It);
		}
	}

	{
		std::list<int>::iterator It = m_edge_ids.begin();
		for(HeeksObj* object = shape->m_edges->GetFirstChild(); object && It != m_edge_ids.end(); object = shape->m_edges->GetNextChild(), It++)
		{
			object->SetID(*It);
		}
	}

	{
		std::list<int>::iterator It = m_vertex_ids.begin();
		for(HeeksObj* object = shape->m_vertices->GetFirstChild(); object && It != m_vertex_ids.end(); object = shape->m_vertices->GetNextChild(), It++)
		{
			object->SetID(*It);
		}
	}
}

