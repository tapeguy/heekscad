// EndedObject.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"

#include "EndedObject.h"
#include "HPoint.h"
#include "MarkedList.h"

EndedObject::EndedObject(const HeeksColor& color){
	m_color = color;
	A = new HPoint(gp_Pnt(), color);
	B = new HPoint(gp_Pnt(), color);

	A->SetSkipForUndo(true);
	B->SetSkipForUndo(true);
}

EndedObject::EndedObject(const EndedObject& e)
{
	A = new HPoint(gp_Pnt(), e.A->GetColor());
	B = new HPoint(gp_Pnt(), e.B->GetColor());
	A->SetSkipForUndo(true);
	B->SetSkipForUndo(true);
	operator=(e);
}

EndedObject::~EndedObject(){
}

const EndedObject& EndedObject::operator=(const EndedObject &b){

	HeeksObj::operator=(b);
	*A = *b.A;
	*B = *b.B;
	m_color = b.m_color;
	return *this;
}

HeeksObj* EndedObject::MakeACopyWithID()
{
	EndedObject* pnew = (EndedObject*)HeeksObj::MakeACopyWithID();
	pnew->m_color = m_color;
	return pnew;
}

bool EndedObject::IsDifferent(HeeksObj *other)
{
	EndedObject* eobj = (EndedObject*)other;
	if(eobj->A->m_p.Distance(A->m_p) > wxGetApp().m_geom_tol)
		return true;

	if(eobj->B->m_p.Distance(B->m_p) > wxGetApp().m_geom_tol)
		return true;

	return HeeksObj::IsDifferent(other);
}

void EndedObject::ModifyByMatrix(const double* m){
	gp_Trsf mat = make_matrix(m);
	A->m_p.Transform(mat);
	B->m_p.Transform(mat);
}

bool EndedObject::Stretch(const double *p, const double* shift, void* data){
	gp_Pnt vp = make_point(p);
	gp_Vec vshift = make_vector(shift);

	if(data == &A){
		A->m_p = vp.XYZ() + vshift.XYZ();
	}
	else if(data == &B){
		B->m_p = vp.XYZ() + vshift.XYZ();
	}
	return false;
}

void EndedObject::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	list->push_back(GripData(GripperTypeStretch,A->m_p.X(),A->m_p.Y(),A->m_p.Z(),&A));
	list->push_back(GripData(GripperTypeStretch,B->m_p.X(),B->m_p.Y(),B->m_p.Z(),&B));
}

bool EndedObject::GetStartPoint(double* pos)
{
	extract(A->m_p, pos);
	return true;
}

bool EndedObject::GetEndPoint(double* pos)
{
	extract(B->m_p, pos);
	return true;
}

void EndedObject::glCommands(bool select, bool marked, bool no_color)
{
	HeeksObj::glCommands(select, marked, no_color);
}

