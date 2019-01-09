// HeeksObj.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include "stdafx.h"
#include "HeeksObj.h"
#include "PropertyFactory.h"
#ifdef HEEKSCAD
#include "../tinyxml/tinyxml.h"
#include "ObjList.h"
#include "../src/Gripper.h"
#include "../src/HeeksFrame.h"
#include "../src/ObjPropsCanvas.h"
#include "../src/Sketch.h"
#else
#include "GripperTypes.h"
#include "GripData.h"
#endif


const TopoDS_Shape empty_shape;


//PropertyDescriptor desc[] = {
//        { PropertyStringType,       _("type"),       _("Object Type"),       { {  PROP_READONLY,  "TRUE"  }, { NULL } }    },
//        { PropertyStringType,       _("title"),      _("Object Title")       },
//        { PropertyIntType,          _("id"),         _("ID")                 },
//        { PropertyColorType,        _("color"),      _("Color")              },
//        { PropertyInvalidType }
//};
//
//
//PropertyString& HeeksObj::GetTypeProperty() const    {   return *((PropertyString *)GetProperty ( _("type") ));      }
//PropertyString& HeeksObj::GetTitleProperty() const   {   return *((PropertyString *)GetProperty ( _("title") ));     }
//PropertyInt& HeeksObj::GetIDProperty() const         {   return *((PropertyInt *)GetProperty ( _("id") ));           }
//PropertyColor& HeeksObj::GetColorProperty() const    {   return *((PropertyColor *)GetProperty ( _("color") ));      }

// : DomainObject(GetShortString((ObjType)obj_type), PropertyFactory(), (unsigned char *)&desc, sizeof(PropertyDescriptor)),


HeeksObj::HeeksObj(int obj_type)
 : DomainObject(GetShortString((ObjType)obj_type)),
   m_owner(NULL),
   m_obj_type(obj_type),
   m_copy_id(false),
   m_skip_for_undo(false),
   m_layer(0),
   m_index(0)
{
//    wxString type = this->GetTypeString();
//    printf ( "Constructing object %s: %lx\n", (const char *)type, this );

    InitializeProperties();
    m_type.SetValue(GetTypeString());
    SetVisible(true);
}

HeeksObj::HeeksObj(const HeeksObj& ho)
 : DomainObject(GetShortString((ObjType)ho.m_obj_type)),
   m_owner(NULL),
   m_obj_type(ho.m_obj_type),
   m_skip_for_undo(false),
   m_layer(0),
   m_index(0)
{
//    wxString type = this->GetTypeString();
//    printf ( "Constructing object %s: %lx (copy)\n", (const char *)type, this );

    InitializeProperties();
    m_copy_id = ho.m_copy_id;
    operator=(ho);
}


HeeksObj::~HeeksObj()
{
//    wxString type = this->GetTypeString();
//    printf ( "Destructing object %s: %lx\n", (const char *)type, this );

    if (m_owner)
    {
        m_owner->Remove(this);
    }

    HeeksCADapp& app = wxGetApp();
    if (&app != NULL)
    {
        if (m_index)
        {
            app.ReleaseIndex(m_index);
        }

        app.RemoveID(this);
    }
}

void HeeksObj::InitializeProperties()
{
    m_type.Initialize(_("type"), this);
    m_type.SetTitle(_("object type"));
    m_type.SetReadOnly(true);

    m_title.Initialize(_("title"), this);
    m_title.SetTitle(_("object title"));

    m_id.Initialize(_("id"), this);
    m_visible.Initialize(_("visible"), this);

    m_color.Initialize(_("color"), this, true);
}

bool HeeksObj::OnPrePropertySet(Property& prop)
{
    if(prop == m_id && (int)m_id != 0)
    {
        OnPreSetID(m_id);
    }

    if(prop == m_title)
    {
        OnPreSetTitle(m_title);
    }
    return true;
}

void HeeksObj::OnPropertySet(Property& prop)
{
    if(prop == m_id && (int)m_id != 0)
    {
        OnSetID(m_id);
    }

    if(prop == m_title)
    {
        OnSetTitle(m_title);
    }

    if (m_owner)
    {
        m_owner->OnChildModified(this, prop);
    }
}

const HeeksObj& HeeksObj::operator=(const HeeksObj &ho)
{
    int id = ho.m_copy_id ? ho.GetID() : GetID();

    for ( DomainObjectIterator it = ho.begin(); it != ho.end(); it++ )
    {
        Property * prop = *it;
        Property * my_prop = this->GetProperty ( prop->GetName ( ) );
        if ( my_prop && my_prop != &m_id )
        {
            *my_prop = *prop;
        }
    }
	m_layer = ho.m_layer;
	m_skip_for_undo = ho.m_skip_for_undo;

	SetID(id);

	return *this;
}

HeeksObj* HeeksObj::MakeACopyWithSameID()
{
    m_copy_id = true;
	HeeksObj* ret = MakeACopy();
	m_copy_id = false;
	return ret;
}

const wxChar* HeeksObj::GetTypeString(void) const
{
    int type = GetType();
    if (type <= ObjectMaximumType)
        return GetShortString((ObjType)type);
    return wxGetApp().HeeksType(GetType());
}

const wxBitmap &HeeksObj::GetIcon()
{
	static wxBitmap* icon = NULL;

	if(icon == NULL)
	{
	    icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/unknown.png")));
	}

	return *icon;
}

bool HeeksObj::GetScaleAboutMatrix(double *m)
{
	// return the bottom left corner of the box
	CBox box;
	GetBox(box);
	if(!box.m_valid)return false;
	gp_Trsf mat;
	mat.SetTranslationPart(gp_Vec(box.m_x[0], box.m_x[1], box.m_x[2]));
	extract(mat, m);
	return true;
}

void HeeksObj::GetGripperPositionsTransformed(std::list<GripData> *list, bool just_for_endof)
{
    GetGripperPositions(list,just_for_endof);
}

bool HeeksObj::Stretch(const double *p, const double* shift, void* data)
{
    double mat[12] = {
            shift[0], 0, 0, p[0],
            0, shift[1], 0, p[1],
            0, 0, shift[2], p[2]
    };
    this->ModifyByMatrix(mat);
    return false;
}

void HeeksObj::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
	CBox box;
	GetBox(box);
	if(!box.m_valid)return;

	//TODO: This is a tab bit of a strange thing to do. Especially for planar objects like faces
	//ones that are on a plane like y-z or x-z will have all gripper merged together.
	list->push_back(GripData(GripperTypeTranslate,box.m_x[0],box.m_x[1],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeRotateObject,box.m_x[3],box.m_x[1],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeRotateObject,box.m_x[0],box.m_x[4],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeScale,box.m_x[3],box.m_x[4],box.m_x[2],NULL));
}

void HeeksObj::GetProperties(std::list<Property *> *list)
{
    if (!UsesID())
        m_id.SetVisible(false);

    if (!UsesColor())
        m_color.SetVisible(false);

    DomainObject::GetProperties ( list );
}

bool HeeksObj::Add(HeeksObj* object, HeeksObj* prev_object /* = NULL */ )
{
	object->m_owner = this;
	object->OnAdd();
	return true;
}

void HeeksObj::OnRemove()
{
	if(m_owner == NULL) {
	    KillGLLists();
	}
}

void HeeksObj::OnPreSetID(ObjectId_t id)
{
    wxGetApp().RemoveID(this);
}

void HeeksObj::OnSetID(ObjectId_t id)
{
    wxGetApp().SetObjectID(this, id);
}

void HeeksObj::WriteXML(TiXmlNode *root)
{
    TiXmlElement * element;
    wxString type = this->GetTypeString();
    type.erase(std::remove(type.begin(), type.end(), ' '), type.end());
    element = new TiXmlElement ( type );
    root->LinkEndChild ( element );
    WriteBaseXML ( element );
}

void HeeksObj::WriteBaseXML(TiXmlElement *element)
{
    std::list<Property *> list;
    GetProperties(&list);

    std::list<Property *>::iterator It;
    for ( It = list.begin(); It != list.end(); It++ )
    {
        Property * prop = *It;
        if ( prop->IsTransient() ||
             prop->GetName() == _("type") ||
             ( prop->GetName() == _("title") && this->GetTitle().empty() ) ||
             ( prop->GetName() == _("id") && ! this->UsesID ( ) ) ||
             ( prop->GetName() == _("visible") && this->IsVisible() ) )
        {
            continue;
        }
        else
        {
            prop->WriteToXmlElement ( element );
        }
    }
}

void HeeksObj::ReadBaseXML(TiXmlElement* element)
{
    std::list<Property *> list;
    GetProperties(&list);

    for ( TiXmlElement* c = element->FirstChildElement ( ); c; c = c->NextSiblingElement ( ) )
    {
        wxString name = c->Value ( );
        Property * prop = FindPropertyByXmlName ( &list, name );
        if ( prop != NULL )
        {
            prop->ReadFromXmlElement ( c );
        }
    }
}

Property * HeeksObj::FindPropertyByXmlName(std::list<Property *> * propList, const wxString& name)
{
    std::list<Property *>::iterator It;
    for ( It = propList->begin(); It != propList->end(); It++ )
    {
        Property * prop = *It;
        if ( prop->GetXmlName() == name )
        {
            return prop;
        }
    }
    return NULL;
}

bool HeeksObj::OnVisibleLayer()
{
	// to do, support multiple layers.
	return true;
}

HeeksObj* HeeksObj::GetOwner()
{
	return m_owner;
}

void HeeksObj::SetOwner(HeeksObj *obj)
{
	m_owner = obj;
}

void HeeksObj::RemoveOwner()
{
	m_owner = NULL;
}
//
//void HeeksObj::ReplaceWithCopy(HeeksObj* new_object, bool call_OnChanged)
//{
//    // Copy all bindings and metadata(such as units)
//    for ( DomainObjectIterator it = new_object->begin(); it != new_object->end(); it++ )
//    {
//        Property * prop = *it;
//        Property * my_prop = this->GetProperty ( prop->GetName ( ) );
//        if (my_prop) {
//            prop->ImportBindings ( *my_prop );
//            prop->CopyMetadata ( *my_prop );
//        }
//    }
//
//    if (this->GetOwner()) {
//        this->GetOwner()->Add(new_object);
//        this->GetOwner()->Remove(this);
//    }
//    if(wxGetApp().m_marked_list->ObjectMarked(this))
//    {
//        wxGetApp().m_marked_list->Remove(this, call_OnChanged);
//        wxGetApp().m_marked_list->Add(new_object, call_OnChanged);
//    }
//    std::set<HeeksObj*>::iterator iter = wxGetApp().m_hidden_for_drag.find(this);
//    if( iter != wxGetApp().m_hidden_for_drag.end()) {
//        wxGetApp().m_hidden_for_drag.erase(iter);
//        wxGetApp().m_hidden_for_drag.insert(new_object);
//    }
//}

const std::list<HeeksObj*>& HeeksObj::GetLinks ( ) const
{
    return m_links;
}

HeeksObj *HeeksObj::Find( const int type, const unsigned int id )
{
	if ((type == this->GetType()) && (id == this->GetID()))
		return(this);
	return(NULL);
}

#ifdef WIN32
#define snprintf _snprintf
#endif

wxString HeeksObj::ToString() const
{
	wxString rtn;
	rtn.Printf("ID: %u, Type: %s, MarkingFilter: 0x%X, IDGroup: 0x%X\n",
	            GetID(), GetShortString((ObjType)GetType()), (unsigned int)GetMarkingFilter(), GetIDGroupType());
	return rtn;
}

unsigned int HeeksObj::GetIndex() {
#ifdef HEEKSCAD
	if (!m_index) m_index = wxGetApp().GetIndex(this);
#else
	if (!m_index) m_index = heeksCAD->GetIndex(this);
#endif
	return m_index;
}
