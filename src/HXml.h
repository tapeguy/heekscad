// HXml.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "../interface/HeeksObj.h"

class HXml: public HeeksObj
{
public:

    static const int ObjType = XmlType;


	TiXmlElement m_element;

	HXml(TiXmlElement* pElem);
    ~HXml(void);

	// HeeksObj's virtual functions
	HeeksObj *MakeACopy(void)const;
	const wxBitmap &GetIcon();
	void CopyFrom(const HeeksObj* object){operator=(*((HXml*)object));}
	void WriteXML(TiXmlNode *root);

	static HeeksObj* ReadFromXMLElement(TiXmlElement* pElem);

};
