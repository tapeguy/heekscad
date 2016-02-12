#pragma once

#include "stdafx.h"

#include "wx/dataview.h"

class wxStaticLine;

class PropBindingSubscriberModelNode;
WX_DEFINE_ARRAY_PTR ( PropBindingSubscriberModelNode*, PropBindingSubscriberModelNodePtrArray );

class PropBindingSubscriberModelNode
{
private:

    PropBindingSubscriberModelNode          *m_parent;
    PropBindingSubscriberModelNodePtrArray   m_children;

public:     // public to avoid getters/setters

    wxString                m_subscriber_name;
    Property *              m_subscriber;
    wxString                m_binding;
    Property *              m_observed;
    bool                    m_container;

    PropBindingSubscriberModelNode ( PropBindingSubscriberModelNode* parent,
                                     const wxString &subscriber_name,
                                     Property * subscriber_prop,
                                     const wxString &binding,
                                     Property * observed_prop,
                                     bool is_container );

    ~PropBindingSubscriberModelNode ( );

    void UpdateNode ( );

    PropBindingSubscriberModelNode* GetParent ( )
        { return m_parent; }
    PropBindingSubscriberModelNodePtrArray& GetChildren ( )
        { return m_children; }
    void Insert ( PropBindingSubscriberModelNode* child, unsigned int n )
        { m_children.Insert ( child, n); }
    void Append ( PropBindingSubscriberModelNode* child )
        { m_children.Add ( child ); }
    unsigned int GetChildCount ( ) const
        { return m_children.GetCount ( ); }
};


class PropBindingSubscriberModel : public wxDataViewModel
{
private:

    PropBindingSubscriberModelNode*   m_root;

public:

    PropBindingSubscriberModel(HeeksObj *obj);

    ~PropBindingSubscriberModel();

    // wxDataViewModel overloads
    virtual unsigned int GetColumnCount ( ) const {
        return 5;
    }

    virtual wxString GetColumnType ( unsigned int col ) const {
        return wxT("string");
    }

    void GetValue ( wxVariant &variant,
                    const wxDataViewItem &item, unsigned int col ) const;
     bool SetValue ( const wxVariant &variant,
                     const wxDataViewItem &item, unsigned int col );

    bool IsEnabled ( const wxDataViewItem &item,
                     unsigned int col ) const;

    wxDataViewItem GetParent ( const wxDataViewItem &item ) const;
    bool IsContainer ( const wxDataViewItem &item ) const;
    unsigned int GetChildren ( const wxDataViewItem &parent,
                               wxDataViewItemArray &array ) const;
//    bool HasContainerColumns ( const wxDataViewItem & item ) const { return true; }
};


class PropBindingObservedModelNode;
WX_DEFINE_ARRAY_PTR ( PropBindingObservedModelNode*, PropBindingObservedModelNodePtrArray );

class PropBindingObservedModelNode
{
private:

    PropBindingObservedModelNode            *m_parent;
    PropBindingObservedModelNodePtrArray     m_children;

public:     // public to avoid getters/setters

    wxString                m_observed_name;
    Property *              m_observed;
    bool                    m_container;

    PropBindingObservedModelNode ( PropBindingObservedModelNode* parent,
                                   const wxString &observed_name,
                                   Property * observed_prop,
                                   bool is_container );

    ~PropBindingObservedModelNode ( );

    PropBindingObservedModelNode* GetParent ( )
        { return m_parent; }
    PropBindingObservedModelNodePtrArray& GetChildren ( )
        { return m_children; }
    void Insert ( PropBindingObservedModelNode* child, unsigned int n )
        { m_children.Insert ( child, n); }
    void Append ( PropBindingObservedModelNode* child )
        { m_children.Add ( child ); }
    unsigned int GetChildCount ( ) const
        { return m_children.GetCount ( ); }
};


class PropBindingObservedModel : public wxDataViewModel
{
private:

    PropBindingObservedModelNode*   m_root;

public:

    PropBindingObservedModel(HeeksObj *obj);

    ~PropBindingObservedModel();

    // wxDataViewModel overloads
    virtual unsigned int GetColumnCount ( ) const {
        return 2;
    }

    virtual wxString GetColumnType ( unsigned int col ) const {
        return wxT("string");
    }

    void GetValue ( wxVariant &variant,
                    const wxDataViewItem &item, unsigned int col ) const;
     bool SetValue ( const wxVariant &variant,
                     const wxDataViewItem &item, unsigned int col );

    bool IsEnabled ( const wxDataViewItem &item,
                     unsigned int col ) const;

    wxDataViewItem GetParent ( const wxDataViewItem &item ) const;
    bool IsContainer ( const wxDataViewItem &item ) const;
    unsigned int GetChildren ( const wxDataViewItem &parent,
                               wxDataViewItemArray &array ) const;
//    bool HasContainerColumns ( const wxDataViewItem & item ) const { return true; }
};


class PropBindingDialog : public wxDialog
{
protected:

    wxDataViewCtrl* prop1;
    PropBindingSubscriberModel* m_subscriber_model;

    wxDataViewCtrl* prop2;
    PropBindingObservedModel* m_observed_model;

    wxButton* m_buttonBind;
    wxButton* m_buttonUnbind;

    wxStaticLine* m_staticline1;
    wxStdDialogButtonSizer* m_buttonSizer;
    wxButton* m_buttonOK;

public:

    PropBindingDialog ( wxWindow* parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& title = _("Edit Property Bindings"),
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxSize( -1,-1 ),
                        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxSTAY_ON_TOP );
    ~PropBindingDialog ( );

    void AddSubscriberModel ( PropBindingSubscriberModel* model );
    void AddObservedModel ( PropBindingObservedModel* model );

    void OnBindButton ( wxCommandEvent& event );
    void OnUnbindButton ( wxCommandEvent& event );

private:

    void ExpandSubscriberRecursive ( PropBindingSubscriberModel* model,
                                     wxDataViewItem& item );
    void ExpandObservedRecursive ( PropBindingObservedModel* model,
                                   wxDataViewItem& item );

    DECLARE_EVENT_TABLE()
};

