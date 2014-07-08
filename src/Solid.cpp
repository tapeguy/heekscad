// Solid.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "Solid.h"
#include "MarkedList.h"

CSolid::CSolid(const TopoDS_Solid &solid, const wxChar* title, const HeeksColor& col, float opacity):CShape(solid, title, col, opacity)
{
}

CSolid::CSolid( const CSolid & rhs ) : CShape(rhs)
{
    *this = rhs;    // Call the assignment operator.
}

CSolid::~CSolid()
{
}

HeeksObj *CSolid::MakeACopy(void)const
{
	return new CSolid(*this);
}

void CSolid::SetXMLElement(TiXmlElement* element)
{
	element->SetAttribute("col", m_color.COLORREF_color());
	if(m_opacity < 0.9999)element->SetDoubleAttribute("opacity", m_opacity);

}

void CSolid::SetFromXMLElement(TiXmlElement* pElem)
{
	for(TiXmlAttribute* a = pElem->FirstAttribute(); a; a = a->Next())
	{
		std::string name(a->Name());
		if(name == "col"){m_color = HeeksColor((long)(a->IntValue()));}
		else if(name == "opacity"){m_opacity = (float)(a->DoubleValue());}
	}
}

const wxBitmap &CSolid::GetIcon()
{
	static wxBitmap* icon = NULL;
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/solid.png")));
	return *icon;
}

void CSolid::MakeTransformedShape(const gp_Trsf &mat)
{
	BRepBuilderAPI_Transform myBRepTransformation(m_shape,mat);
	m_shape = myBRepTransformation.Shape();
}
