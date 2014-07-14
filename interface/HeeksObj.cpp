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
#include "../src/Pad.h"
#else
#include "GripperTypes.h"
#include "GripData.h"
#endif



PropertyDescriptor desc[] = {
        { PropertyStringType,       _("type"),       _("Object Type"),       { {  PROP_READONLY,  "TRUE"  }, { NULL } }    },
        { PropertyStringType,       _("title"),      _("Object Title")       },
        { PropertyIntType,          _("id"),         _("ID")                 },
        { PropertyCheckType,        _("visible"),    _("Visible")            },
        { PropertyInvalidType }
};


PropertyString& HeeksObj::GetTypeProperty() const    {   return *((PropertyString *)GetProperty ( _("type") ));      }
PropertyString& HeeksObj::GetTitleProperty() const   {   return *((PropertyString *)GetProperty ( _("title") ));     }
PropertyInt& HeeksObj::GetIDProperty() const         {   return *((PropertyInt *)GetProperty ( _("id") ));           }
PropertyCheck& HeeksObj::GetVisibleProperty() const  {   return *((PropertyCheck *)GetProperty ( _("visible") ));    }


HeeksObj::HeeksObj(void)
 : DomainObject(GetTypeString(), PropertyFactory(), (unsigned char *)&desc, sizeof(PropertyDescriptor)),
   m_owner(NULL),
   m_skip_for_undo(false),
   m_layer(0),
   m_preserving_id(false),
   m_index(0)
{
    GetTypeProperty().SetValue(GetTypeString());
    SetTitle(_(""));
    SetID(0);
    SetVisible(true);

	InitializeProperties();
}

HeeksObj::HeeksObj(const HeeksObj& ho)
 : DomainObject(GetTypeString(), PropertyFactory(), (unsigned char *)&desc, sizeof(PropertyDescriptor)),
   m_owner(NULL),
   m_skip_for_undo(false),
   m_layer(0),
   m_preserving_id(false),
   m_index(0)
{
	InitializeProperties();
	operator=(ho);
}

void HeeksObj::InitializeProperties()
{
}

bool HeeksObj::OnPropertySet(Property& prop)
{
	if(prop == GetTitleProperty()) {
		OnEditString(GetTitleProperty());
	}
	else if(prop == GetIDProperty() && GetID() != 0) {
		OnSetID(GetIDProperty() );
	}
	return TRUE;
}

const HeeksObj& HeeksObj::operator=(const HeeksObj &ho)
{
    int id = ho.m_preserving_id ? ho.GetID() : GetID();

    for ( DomainObjectIterator itr = ho.begin(); itr != ho.end(); itr++ )
    {
        Property * prop = *itr;
        Property * my_prop = this->GetProperty ( prop->GetName ( ) );
        if ( my_prop )
        {
            *my_prop = *prop;
        }
//        else
//        {
//            this->AddProperty ( (Property *)prop.Clone ( ) );
//        }
    }
	m_layer = ho.m_layer;
	m_skip_for_undo = ho.m_skip_for_undo;

	SetID(id);

	return *this;
}

HeeksObj::~HeeksObj()
{
	if(m_owner)
	    m_owner->Remove(this);

#ifdef HEEKSCAD
	if (m_index) wxGetApp().ReleaseIndex(m_index);
#else
	if (m_index) heeksCAD->ReleaseIndex(m_index);
#endif
}

HeeksObj* HeeksObj::MakeACopyWithID()
{
	m_preserving_id = true;
	HeeksObj* ret = MakeACopy();
	m_preserving_id = false;
	return ret;
}

const wxBitmap &HeeksObj::GetIcon()
{
	static wxBitmap* icon = NULL;
#ifdef HEEKSCAD
	if(icon == NULL)icon = new wxBitmap(wxImage(wxGetApp().GetResFolder() + _T("/icons/unknown.png")));
#else
	if(icon == NULL)icon = new wxBitmap(wxImage(heeksCAD->GetResFolder() + _T("/icons/unknown.png")));
#endif
	return *icon;
}

bool HeeksObj::GetScaleAboutMatrix(double *m)
{
#ifdef HEEKSCAD
	// return the bottom left corner of the box
	CBox box;
	GetBox(box);
	if(!box.m_valid)return false;
	gp_Trsf mat;
	mat.SetTranslationPart(gp_Vec(box.m_x[0], box.m_x[1], box.m_x[2]));
	extract(mat, m);
	return true;
#else
	return false;
#endif
}

bool HeeksObj::StretchTemporaryTransformed(const double *p, const double* shift, void* data)
{
#ifdef HEEKSCAD
	gp_Trsf mat;

	HeeksObj* owner = m_owner;
	CSketch *sketch = dynamic_cast<CSketch*>(owner);

	if(sketch && sketch->m_coordinate_system)
		mat = sketch->m_coordinate_system->GetMatrix();

	//mat.Invert();

	gp_Pnt vp = make_point(p);
	gp_Vec vshift = make_vector(shift);

	//vp.Transform(mat);
	//vshift.Transform(mat);

	double np[3];
	double nshift[3];
	extract(vp,np);
	extract(vshift,nshift);

	return StretchTemporary(np,nshift,data);
#else
	return StretchTemporary(p,shift,data);
#endif
}

void HeeksObj::GetGripperPositionsTransformed(std::list<GripData> *list, bool just_for_endof)
{
#ifdef HEEKSCAD

	//TODO: We want to transform these coords by whatever has happened to the draw matrix on the way down to our level
	//For right now we are just grabbing the sketches coord system, but this isn't right and won't work when parts or
	//assemblies come around.
	//For that matter it has gotten out of control with the addition of faces and edges to pads
	std::list<GripData> newlist;
	GetGripperPositions(&newlist,just_for_endof);

	gp_Trsf mat;

	CSketch *sketch = dynamic_cast<CSketch*>(m_owner);

	if(sketch && sketch->m_coordinate_system)
		mat = sketch->m_coordinate_system->GetMatrix();

	CPad *pad = dynamic_cast<CPad*>(m_owner);
	if(!pad && m_owner)
		pad = dynamic_cast<CPad*>(m_owner->m_owner);

	if(pad && pad->m_sketch->m_coordinate_system)
		mat = pad->m_sketch->m_coordinate_system->GetMatrix();

	std::list<GripData>::iterator it;
	for(it = newlist.begin(); it != newlist.end(); ++it)
	{
		GripData gd = *it;

		gp_Pnt pnt(gd.m_x,gd.m_y,gd.m_z);
		pnt.Transform(mat);
		gd.m_x = pnt.X();
		gd.m_y = pnt.Y();
		gd.m_z = pnt.Z();
		list->push_back(gd);
	}
#else
	GetGripperPositions(list,just_for_endof);
#endif
}

void HeeksObj::GetGripperPositions(std::list<GripData> *list, bool just_for_endof)
{
//#ifdef HEEKSCAD
	CBox box;
	GetBox(box);
	if(!box.m_valid)return;

	//TODO: This is a tab bit of a strange thing to do. Especially for planar objects like faces
	//ones that are on a plane like y-z or x-z will have all gripper merged together.
	list->push_back(GripData(GripperTypeTranslate,box.m_x[0],box.m_x[1],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeRotateObject,box.m_x[3],box.m_x[1],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeRotateObject,box.m_x[0],box.m_x[4],box.m_x[2],NULL));
	list->push_back(GripData(GripperTypeScale,box.m_x[3],box.m_x[4],box.m_x[2],NULL));
//#endif
}

void HeeksObj::GetProperties(std::list<Property *> *list)
{
    GetTypeProperty().SetValue(GetTypeString());
    if (!UsesID())
        GetIDProperty().SetVisible(false);

    DomainObject::GetProperties ( list );
}

bool HeeksObj::Add(HeeksObj* object, HeeksObj* prev_object)
{
	object->m_owner = this;
	object->OnAdd();
	return true;
}

void HeeksObj::OnRemove()
{
	if(m_owner == NULL)
	    KillGLLists();
}

void HeeksObj::OnSetID(int id)
{
#ifdef HEEKSCAD
	wxGetApp().SetObjectID(this, id);
#else
	heeksCAD->SetObjectID(this, id);
#endif
}

void HeeksObj::WriteBaseXML(TiXmlElement *element)
{
#ifdef HEEKSCAD
	wxGetApp().ObjectWriteBaseXML(this, element);
#else
	heeksCAD->ObjectWriteBaseXML(this, element);
#endif
}

#ifdef CONSTRAINT_TESTER
//JT
void HeeksObj::AuditHeeksObjTree4Constraints( HeeksObj * SketchPtr ,HeeksObj * mom, int level,bool ShowMsgInConsole,bool * constraintError)
{

    //If this routine is firing it probably means that either this has nothing to do with constraints or
    //a virtual function was not implemented in a function.
    //Most of the information needed can be fulled from ConstrainedObject::AuditHeeksObjTree4Constraints

    wxString message=wxString::Format(wxT("Level:%d  %s ID=%d (From HeekObj::AuditHeeksObjTree4Constraints") ,level,GetTypeString(),m_id);
    if (ShowMsgInConsole)wxPuts(message);

}

void HeeksObj::HeeksObjOccurrenceInSketch(HeeksObj * Sketch,HeeksObj * Object, int * OccurenceOfObjectInSketch,int FromLevel,bool ShowMsgInConsole)
{
    *OccurenceOfObjectInSketch =0;
    if (Sketch!=NULL)
    Sketch->FindConstrainedObj(Sketch,Object,OccurenceOfObjectInSketch,FromLevel,0,ShowMsgInConsole);//This initiates the recursion
    else
    wxMessageBox(_("Sketch Pointer == NULL in HeeksObjOccurrenceInSketch"));


}
void HeeksObj::FindConstrainedObj(HeeksObj * CurrentObject,HeeksObj * ObjectToFind,int * OccurenceOfObjectInSketch,int FromLevel,int Level,bool ShowMsgInConsole)
{
    //if we hit this it's the end of the line
    if (this == ObjectToFind)
    {
       (*OccurenceOfObjectInSketch)++;
    }

}


#endif

void HeeksObj::ReadBaseXML(TiXmlElement* element)
{
#ifdef HEEKSCAD
	wxGetApp().ObjectReadBaseXML(this, element);
#else
	heeksCAD->ObjectReadBaseXML(this, element);
#endif
}

bool HeeksObj::OnVisibleLayer()
{
	// to do, support multiple layers.
	return true;
}


HeeksObj* HeeksObj::Owner()
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

void HeeksObj::ToString(char *str, unsigned int* rlen, unsigned int len)
{
	unsigned int printed;
	*rlen = 0;

	printed = snprintf(str,len,"ID: 0x%X, Type: 0x%X, MarkingFilter: 0x%X, IDGroup: 0x%X\n",GetID(),GetType(),(unsigned int)GetMarkingFilter(),GetIDGroupType());
	if(printed >= len)
		goto abort;
	*rlen += printed; len -= printed;

abort:
	*rlen = 0;
}

unsigned int HeeksObj::GetIndex() {
#ifdef HEEKSCAD
	if (!m_index) m_index = wxGetApp().GetIndex(this);
#else
	if (!m_index) m_index = heeksCAD->GetIndex(this);
#endif
	return m_index;
}
