// Part.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "DynamicSolid.h"

class CPart: public DynamicSolid{
public:

	CPart();
	~CPart();

	const wxChar* GetTypeString(void)const{return _("Part");}
	int GetType()const{return PartType;}
	int GetMarkingFilter()const{return PartMarkingFilter;}
	HeeksObj *MakeACopy(void)const{ return new CPart(*this);}
	void glCommands(bool select, bool marked, bool no_color);
	void Update();
};

