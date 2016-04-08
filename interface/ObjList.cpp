// ObjList.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.
#include "stdafx.h"
#include "ObjList.h"
#ifdef HEEKSCAD
#include "../src/MarkedList.h"
#include "../tinyxml/tinyxml.h"
#else
#include "HeeksCADInterface.h"
#endif

#include <algorithm>

ObjList::ObjList(int obj_type)
: HeeksObj(obj_type), did_remove(false), m_index_list_valid(true)
{
}

ObjList::ObjList(const ObjList& objlist)
: HeeksObj(objlist), did_remove(false), m_index_list_valid(true)
{
    operator=(objlist);
}

void ObjList::Clear()
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		(*It)->RemoveOwner();
		delete *It;
	}
	m_objects.clear();
	m_index_list.clear();
	m_index_list_valid = true;
}

void ObjList::Clear(std::set<HeeksObj*> &to_delete)
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end();)
	{
		if(to_delete.find(*It) != to_delete.end())
		{
			(*It)->RemoveOwner();
			It = m_objects.erase(It);
		}
		else
			It++;
	}
	m_index_list.clear();
	m_index_list_valid = false;
}

const ObjList& ObjList::operator=(const ObjList& objlist)
{
    HeeksObj::operator=(objlist);
    Clear();
    std::list<HeeksObj*>::const_iterator It;
    for (It=objlist.m_objects.begin(); It!=objlist.m_objects.end(); It++)
    {
        if(!(*It)->OneOfAKind())
        {
            HeeksObj* new_op = m_copy_id ? (*It)->MakeACopyWithSameID() : (*It)->MakeACopy();
            if(new_op)
            {
                Add(new_op, NULL);
            }
        }
    }
    return *this;
}

void ObjList::ClearUndoably(void)
{
	if (m_objects.size() == 0) return;
	std::list<HeeksObj*> objects_to_delete = m_objects;
	std::list<HeeksObj*>::iterator It;
	for (It=objects_to_delete.begin();It!=objects_to_delete.end();It++)
	{
#ifdef HEEKSCAD
		wxGetApp().DeleteUndoably(*It);
#else
		heeksCAD->DeleteUndoably(*It);
#endif
	}
	m_objects.clear();
	LoopItStack.clear();
	m_index_list.clear();
	m_index_list_valid = true;
}

void ObjList::PropagateProperty(const Property& prop)
{
    std::list<HeeksObj*>::iterator It;
    for(It=m_objects.begin(); It!=m_objects.end() ;It++)
    {
        HeeksObj* object = *It;
        Property * other_prop = object->GetProperty(prop.GetName());
        if (other_prop && prop.GetPropertyType() == other_prop->GetPropertyType()) {
            *other_prop = prop;
        }
    }
}

HeeksObj* ObjList::MakeACopy(void) const
{
    return new ObjList(*this);
}

void ObjList::GetBox(CBox &box)
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		if(object->OnVisibleLayer() && object->IsVisible())
		{
			object->GetBox(box);
		}
	}
}

void ObjList::glCommands(bool select, bool marked, bool no_color)
{
	if(!IsVisible())
		return;
	HeeksObj::glCommands(select, marked, no_color);
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		if(object->OnVisibleLayer() && object->IsVisible())
		{
			if(select)
			    glPushName(object->GetIndex());
			(*It)->glCommands(select, marked || wxGetApp().m_marked_list->ObjectMarked(object), no_color);
			if(select)
			    glPopName();
		}
	}
}

void ObjList::Draw(wxDC& dc)
{
	HeeksObj::Draw(dc);
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj* object = *It;
		if(object->OnVisibleLayer() && object->IsVisible())
		{
			object->Draw(dc);
		}
	}
}

HeeksObj* ObjList::GetFirstChild()
{
	if (m_objects.size()==0)
	    return NULL;
	LoopIt = m_objects.begin();
	did_remove = false;
	return *LoopIt;
}

HeeksObj* ObjList::GetNextChild()
{
	if (m_objects.size()==0 || LoopIt==m_objects.end())
	    return NULL;
	if (!did_remove)
	    LoopIt++;
	if (LoopIt==m_objects.end())
	    return NULL;
	return *LoopIt;
}

void ObjList::recalculate_index_list()
{
	m_index_list.clear();
	m_index_list.resize(m_objects.size());
	int i = 0;
	for(std::list<HeeksObj*>::iterator It=m_objects.begin(); It!=m_objects.end() ;It++, i++)
	{
		HeeksObj* object = *It;
		m_index_list[i] = object;
	}
	m_index_list_valid = true;
}

HeeksObj* ObjList::GetAtIndex(int index)
{
	if(!m_index_list_valid)
	{
		recalculate_index_list();
	}

	if(index < 0 || index >= (int)(m_index_list.size()))return NULL;
	return m_index_list[index];
}

int ObjList::GetNumChildren()
{
	return m_objects.size();
}

void ObjList::Add(std::list<HeeksObj*> objects)
{
	std::list<HeeksObj*>::iterator it;
	for(it = objects.begin(); it != objects.end(); it++)
	{
		Add(*it,NULL);
	}
}

void ObjList::Remove(std::list<HeeksObj*> objects)
{
	std::list<HeeksObj*>::iterator it;
	for(it = objects.begin(); it != objects.end(); it++)
	{
		Remove(*it);
	}
}

bool ObjList::Add(HeeksObj* object, HeeksObj* prev_object /* = NULL */)
{
	if (object==NULL)
	    return false;
	if (!CanAdd(object))
	    return false;
	if (std::find(m_objects.begin(), m_objects.end(), object) != m_objects.end())
	    return true; // It's already here.

	if (m_objects.size()==0 || prev_object==NULL)
	{
		m_objects.push_back(object);
		LoopIt = m_objects.end();
		LoopIt--;
	}
	else
	{
		for(LoopIt = m_objects.begin(); LoopIt != m_objects.end(); LoopIt++)
		{
		    if (*LoopIt==prev_object)
		        break;
		}
		m_objects.insert(LoopIt, object);
	}
    did_remove = false;
	m_index_list_valid = false;
	HeeksObj::Add(object, prev_object);

	if(((!wxGetApp().m_in_OpenFile ||
	      wxGetApp().m_file_open_or_import_type != FileOpenTypeHeeks ||
	      wxGetApp().m_inPaste) &&
	     object->UsesID() && (object->GetID() == 0 ||
	       (wxGetApp().m_file_open_or_import_type == FileImportTypeHeeks && wxGetApp().m_in_OpenFile))))
	{
		object->SetID(wxGetApp().GetNextID(object->GetIDGroupType()));
	}

	return true;
}


std::list<HeeksObj *> ObjList::GetChildren() const
{
	std::list<HeeksObj *> children;
	std::copy( m_objects.begin(), m_objects.end(), std::inserter(children, children.begin()) );
	return(children);
}


void ObjList::Remove(HeeksObj* object)
{
	if (object==NULL)
	    return;
	for(LoopIt = m_objects.begin(); LoopIt != m_objects.end(); LoopIt++)
	{
		if(*LoopIt==object)
		    break;
	}
	if(LoopIt != m_objects.end())
	{
	    LoopIt = m_objects.erase(LoopIt);
        did_remove = true;
	}
	m_index_list_valid = false;
	HeeksObj::Remove(object);

	std::list<HeeksObj*> parents;
	parents.push_back(this);
	object->Disconnect(parents);

	if( (!wxGetApp().m_in_OpenFile || wxGetApp().m_file_open_or_import_type != FileOpenTypeHeeks) &&
		object->UsesID() &&
		(object->GetID() == 0 || (wxGetApp().m_file_open_or_import_type == FileImportTypeHeeks && wxGetApp().m_in_OpenFile))
		)
	{
		wxGetApp().RemoveID(object);
	}
}

void ObjList::KillGLLists(void)
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
    {
	    (*It)->KillGLLists();
    }
}

void ObjList::WriteBaseXML(TiXmlElement *element)
{
	std::list<HeeksObj*>::iterator It;
	for(It=m_objects.begin(); It!=m_objects.end() ;It++)
    {
	    (*It)->WriteXML((TiXmlNode*)element);
    }
	HeeksObj::WriteBaseXML(element);
}

void ObjList::ReadBaseXML(TiXmlElement* element)
{
	// loop through all the objects
	for(TiXmlElement* pElem = TiXmlHandle(element).FirstChildElement().Element(); pElem; pElem = pElem->NextSiblingElement())
	{
	    HeeksObj *existing = NULL;

		HeeksObj* object = wxGetApp().ReadXMLElement(pElem);

        wxString name = pElem->Value ( );
        Property * my_prop = this->GetProperty ( name );
        if ( my_prop )
        {
            my_prop->ReadFromXmlElement ( pElem );
        }
        else if ( object )
		{
            if ((object->GetType() != 0) && (object->GetID() != 0))
            {
                existing = wxGetApp().GetIDObject( object->GetType(), object->GetID() );
            }

            // Check to see if this object has not already been read into memory (duplicate child of two parents)
            // If so, use the existing object rather than this one.
            if ((existing != NULL) && (existing != object))
            {
                Add(existing, NULL); // Add the pre-existing object as a child of this object.
                delete object;
            }
            else
            {
                // Add the new object.
                Add(object, NULL);
            }
		}
	}

	HeeksObj::ReadBaseXML(element);
}

void ObjList::ModifyByMatrix(const double *m)
{
	std::list<HeeksObj*> copy_list = m_objects;
	for(std::list<HeeksObj*>::iterator It=copy_list.begin(); It!=copy_list.end() ;It++)
	{
		(*It)->ModifyByMatrix(m);
	}
}

void ObjList::GetTriangles(void(*callbackfunc)(const double* x, const double* n), double cusp, bool just_one_average_normal)
{
	for(std::list<HeeksObj*>::iterator It=m_objects.begin(); It!=m_objects.end() ;It++) (*It)->GetTriangles(callbackfunc, cusp, just_one_average_normal);
}

/**
	This is the overload for the corresponding method in the HeeksObj class.  It looks for an existing
	object anywhere in this or the child elements (or their children's children.... or their
	children's children's children....you get the idea)

	The idea is to search for pre-existing objects when we're importing a new set of data.  If none
	can be found then a NULL should be returned.
 */
HeeksObj *ObjList::Find( const int type, const unsigned int id )
{
	if ((type == this->GetType()) && (id == this->GetID())) return(this);
	for(std::list<HeeksObj*>::const_iterator It=m_objects.begin(); It!=m_objects.end() ;It++)
	{
		HeeksObj *object = (*It)->Find( type, id );
		if (object != NULL) return(object);
	} // End for
	return(NULL);
}

void ObjList::ReloadPointers()
{
	for (std::list<HeeksObj*>::iterator itObject = m_objects.begin(); itObject != m_objects.end(); itObject++)
	{
		(*itObject)->ReloadPointers();
	}

	HeeksObj::ReloadPointers();
}

bool ObjList::operator==( const ObjList & rhs ) const
{
	return(true);


	if (m_objects.size() != rhs.m_objects.size()) return(false);

	std::list<const HeeksObj *> sorted_lhs;
	std::copy( m_objects.begin(), m_objects.end(), std::inserter( sorted_lhs, sorted_lhs.begin() ) );

	std::list<const HeeksObj *> sorted_rhs;
	std::copy( rhs.m_objects.begin(), rhs.m_objects.end(), std::inserter( sorted_rhs, sorted_rhs.begin() ) );

	sorted_lhs.sort();
	sorted_rhs.sort();

	std::list<const HeeksObj *> difference;

	// std::set_difference( sorted_lhs.begin(), sorted_lhs.end(), sorted_rhs.begin(), sorted_rhs.begin(), difference.begin() );
	// return(difference.size() == 0);

	return(sorted_lhs == sorted_rhs);
}

void ObjList::OnChangeViewUnits(const EnumUnitType units)
{
	for (std::list<HeeksObj*>::iterator itObject = m_objects.begin(); itObject != m_objects.end(); itObject++)
	{
		(*itObject)->OnChangeViewUnits(units);
	}

	HeeksObj::OnChangeViewUnits(units);
}


wxString ObjList::ToString() const
{
    wxString rtn = HeeksObj::ToString();

    for (std::list<HeeksObj*>::const_iterator itObject = m_objects.begin(); itObject != m_objects.end(); itObject++)
    {
        rtn += _("\tchild: ") + (*itObject)->ToString();
    }
    return rtn;
}

ReorderTool::ReorderTool(ObjList* object, std::list<HeeksObj *> &new_order)
{
    m_object = object;
    m_original_order = object->m_objects;
    m_new_order = new_order;
}

void ReorderTool::Run(bool redo)
{
    m_object->m_objects = m_new_order;
}

void ReorderTool::RollBack()
{
    m_object->m_objects = m_original_order;
}
