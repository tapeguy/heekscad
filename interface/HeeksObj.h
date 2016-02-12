// HeeksObj.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#pragma once

#include "Box.h"
#include "wx/dc.h"

#include <list>
#include "DomainObject.h"
#include "Property.h"
#include "ObjTypes.h"

class HeeksColor;
class Tool;
class MarkedObject;
class TiXmlNode;
class TiXmlElement;
class GripData;
class TopoDS_Shape;
class ObjectCanvas;


#ifdef HEEKSCAD
#define GET_ICON(X,Y) x = (X); y = (Y); texture_number = wxGetApp().m_icon_texture_number
#else
#define GET_ICON(X,Y) x = (X); y = (Y); texture_number = theApp.m_icon_texture_number
#endif

extern const TopoDS_Shape empty_shape;

typedef unsigned int ObjectId_t;


class HeeksObj : public DomainObject {

private:

	HeeksObj* m_owner;
	std::list<HeeksObj*> m_links;
	int m_obj_type;
	PropertyString m_type;
	PropertyString m_title;
	PropertyInt m_id;
	PropertyCheck m_visible;
	PropertyColor m_color;

protected:

    bool m_copy_id;

    // Properties
    PropertyString&   GetTypeProperty() { return m_type; }
    PropertyString&   GetTitleProperty() { return m_title; }
    PropertyInt&      GetIDProperty() { return m_id; }
    PropertyCheck&    GetVisibleProperty() { return m_visible; }
    PropertyColor&    GetColorProperty() { return m_color; }

public:

    bool m_skip_for_undo;
    unsigned int m_layer;

    unsigned int m_index;

    HeeksObj(int obj_type);
    HeeksObj(const HeeksObj& copy);

	virtual ~HeeksObj();

	virtual const HeeksObj& operator=(const HeeksObj &ho);

	// Property call-backs
	virtual bool OnPrePropertySet(Property& prop);
	void OnPropertySet(Property& prop);

	// ID
    virtual ObjectId_t GetID() const { ObjectId_t id = (int)m_id; return id; }
    virtual void SetID(ObjectId_t id) { m_id = (int)id; }
    virtual void OnPreSetID(ObjectId_t id);
    virtual void OnSetID(ObjectId_t id);
    virtual bool UsesID() { return true; }

    // Title
    virtual const wxString& GetTitle() const { return m_title; }
    virtual void SetTitle(const wxChar* title) { m_title.SetValue(title); }
    virtual void OnPreSetTitle(const wxString& title) { }
    virtual void OnSetTitle(const wxString& title) { }

    // Visible
    virtual bool IsVisible() const { return m_visible.IsSet(); }
    virtual void SetVisible(bool visible) { m_visible.SetValue(visible); }

    // Color
    virtual const HeeksColor& GetColor() const { return m_color; }
    virtual void SetColor(const HeeksColor& color) { m_color.SetValue(color); }
    virtual bool UsesColor() { return false; }

    // Owner
    HeeksObj* GetOwner();
    virtual void SetOwner(HeeksObj*);
    virtual void RemoveOwner();
    virtual void OnChildModified(HeeksObj * child, Property& prop) { }

	// virtual functions
	virtual void InitializeProperties();
    virtual void GetProperties(std::list<Property *> *list);

	virtual int GetType()const { return m_obj_type;}
    virtual const wxChar* GetTypeString(void) const;
	virtual int GetMarkingFilter()const { return UnknownMarkingFilter; }
	virtual int GetIDGroupType()const { return GetType();}
	virtual void glCommands(bool select, bool marked, bool no_color) { };
	virtual void Draw(wxDC& dc) { } // for printing
	virtual bool DrawAfterOthers() { return false; }
	virtual void GetBox(CBox &box) { }
	virtual void KillGLLists(void) { };
	virtual HeeksObj *MakeACopy() const = 0;
	virtual HeeksObj *MakeACopyWithSameID();
	virtual void ReloadPointers() { }
	virtual void Disconnect(std::list<HeeksObj*>parents) { }
	virtual void CopyFrom(const HeeksObj* object) { }
	virtual void ModifyByMatrix(const double *m) { } // transform the object
	virtual bool GetStartPoint(double* pos) { return false; }
	virtual bool GetEndPoint(double* pos) { return false; }
	virtual bool GetCentrePoint(double* pos) { return false; }
	virtual int GetCentrePoints(double* pos, double* pos2) { return (GetCentrePoint(pos)) ? 1 : 0; }
	virtual bool GetMidPoint(double* pos) { return false; }
	virtual bool GetScaleAboutMatrix(double *m);
	virtual void GetOnEdit(bool(**callback)(HeeksObj*)) { } // returns a function for doing edit with a dialog
    bool Edit ( )
    {
        bool (*fn) ( HeeksObj* ) = NULL;
        GetOnEdit ( &fn );
        if ( fn )
        {
            if ( ( *fn ) ( this ) )
            {
                WriteDefaultValues ( );
                return true;
            }
        }
        return false;
    }  // do edit with a dialog
	virtual const wxBitmap &GetIcon();
	virtual int Intersects(const HeeksObj *object, std::list< double > *rl) const { return 0; }
	virtual bool FindNearPoint(const double* ray_start, const double* ray_direction, double *point) { return false; }
	virtual bool FindPossTangentPoint(const double* ray_start, const double* ray_direction, double *point) { return false; }
	virtual void GetTools(std::list<Tool*>* t_list, const wxPoint* p) { }
	virtual void GetGripperPositionsTransformed(std::list<GripData> *list, bool just_for_endof);
	virtual bool Stretch(const double *p, const double* shift, void* data) { return false; } // return true, if undo stretch is done with Add and Delete
	virtual bool StretchTemporary(const double *p, const double* shift, void* data) { Stretch(p, shift, data); return true; } // returns true, because Stretch was done.  If not done, then override and return false;
	virtual void SetClickMarkPoint(MarkedObject* marked_object, const double* ray_start, const double* ray_direction) { }
	virtual bool CanAdd(HeeksObj* object) { return false; }
	virtual bool CanAddTo(HeeksObj* owner) { return true; }
	virtual bool DescendForUndo() { return true; }
	virtual bool GetSkipForUndo() { return m_skip_for_undo;}
	virtual void SetSkipForUndo(bool val) { m_skip_for_undo = val; }
	virtual bool OneOfAKind() { return false; } // if true, then, instead of pasting, find the first object of the same type and copy object to it.
	virtual bool Add(HeeksObj* object, HeeksObj* prev_object = NULL);
	virtual bool IsDifferent(HeeksObj* other) { return false; }
	virtual void Remove(HeeksObj* object) { object->OnRemove(); }
	virtual void OnAdd() { }
	virtual void OnRemove();
	virtual bool CanBeRemoved() { return true; }
	virtual bool CanBeDragged() { return true; }
	virtual bool CanBeCopied() { return true; }
	virtual HeeksObj* GetFirstChild() { return NULL; }
	virtual HeeksObj* GetNextChild() { return NULL; }
	virtual HeeksObj* GetAtIndex(int index) { return NULL; }
	virtual int GetNumChildren() { return 0; }
	virtual std::list<HeeksObj *> GetChildren() const { std::list<HeeksObj *> empty; return(empty); }
	virtual bool AutoExpand() { return false; }
	virtual void GetTriangles(void(*callbackfunc)(const double* x, const double* n), double cusp, bool just_one_average_normal = true) { } // [nine doubles, three doubles],  or [nine doubles, nine doubles] if just_one_average_normal = false
	virtual double Area()const { return 0.0; }
	virtual void GetSegments(void(*callbackfunc)(const double *p), double pixels_per_mm, bool want_start_point = true) const { };
	virtual void WriteXML(TiXmlNode *root);
	virtual void WriteBaseXML(TiXmlElement *element);
	virtual void ReadBaseXML(TiXmlElement* element);
	bool OnVisibleLayer();
    const std::list<HeeksObj*>& GetLinks ( ) const;
	virtual const TopoDS_Shape& GetShape() const { return(empty_shape); }
	virtual bool IsList(){return false;}
	virtual HeeksObj *Find( const int type, const unsigned int id );
	virtual wxString ToString() const;
	virtual unsigned int GetIndex();
	virtual bool UsesCustomSubNames() { return false; }

protected:
	virtual void GetGripperPositions(std::list<GripData> *list, bool just_for_endof);
//	virtual void ReplaceWithCopy(HeeksObj* new_object, bool call_OnChanged);

public:
	virtual void OnChangeViewUnits(const EnumUnitType units) { }
    virtual void WriteDefaultValues() { }
    virtual void ReadDefaultValues() { }
};
