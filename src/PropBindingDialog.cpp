
#include "PropBindingDialog.h"
#include <wx/dataview.h>
#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////

enum
{
        ID_BIND = 100,
        ID_UNBIND
};

BEGIN_EVENT_TABLE(PropBindingDialog, wxDialog)
    EVT_BUTTON(ID_BIND, PropBindingDialog::OnBindButton)
    EVT_BUTTON(ID_UNBIND, PropBindingDialog::OnUnbindButton)
END_EVENT_TABLE()


PropBindingSubscriberModelNode::PropBindingSubscriberModelNode ( PropBindingSubscriberModelNode* parent,
                                                                 const wxString &subscriber_name,
                                                                 Property * subscriber_prop,
                                                                 const wxString &binding,
                                                                 Property * observed_prop,
                                                                 bool is_container )
 : m_parent ( parent ),
   m_subscriber_name ( subscriber_name ),
   m_subscriber ( subscriber_prop ),
   m_binding ( binding ),
   m_observed ( observed_prop ),
   m_container ( is_container )
{
}

PropBindingSubscriberModelNode::~PropBindingSubscriberModelNode ( )
{
    // free all our children nodes
    size_t count = m_children.GetCount();
    for (size_t i = 0; i < count; i++) {
        PropBindingSubscriberModelNode *child = m_children[i];
        delete child;
    }
}

void PropBindingSubscriberModelNode::UpdateNode ( )
{
    m_subscriber_name = m_subscriber->GetName ( );
    Binding * binding = m_subscriber->GetBinding ( );
    if ( binding ) {
        m_binding = binding->GetPropertyTypeName ( );
        m_observed = binding->GetOtherProperty ( m_subscriber );
    }
    else {
        m_binding = wxEmptyString;
        m_observed = NULL;
    }
    m_container = m_subscriber->IsContainer ( );
}

PropBindingSubscriberModel::PropBindingSubscriberModel ( HeeksObj *obj )
{
    wxString str = obj->GetTitle ( );
    if ( str.IsEmpty ( ) ) {
            str = obj->GetTypeString ( );
    }

    m_root = new PropBindingSubscriberModelNode ( NULL, str, NULL, wxEmptyString, NULL, true );
    for ( DomainObjectIterator it = obj->begin ( ); it != obj->end ( ); it++ )
    {
        Property * prop = *it;
        if ( prop->IsBindable ( ) ) {
            PropBindingSubscriberModelNode* node;
            Binding * binding = prop->GetBinding ( );
            if ( binding ) {
                wxString binding_type = binding->GetPropertyTypeName ( );
                Property * observed = binding->GetOtherProperty ( prop );
                node = new PropBindingSubscriberModelNode (
                        m_root, prop->GetName ( ), prop, binding_type, observed, prop->IsContainer ( ) );
            }
            else
            {
                node = new PropBindingSubscriberModelNode (
                        m_root, prop->GetName ( ), prop, wxEmptyString, NULL, prop->IsContainer ( ) );
            }
            m_root->Append ( node );
        }
    }
}

PropBindingSubscriberModel::~PropBindingSubscriberModel ( )
{
    delete m_root;
}

void PropBindingSubscriberModel::GetValue ( wxVariant &variant,
                                            const wxDataViewItem &item, unsigned int col ) const
{
    wxASSERT(item.IsOk());

    PropBindingSubscriberModelNode *node = (PropBindingSubscriberModelNode*) item.GetID();
    switch (col) {
    case 0:
        variant = node->m_subscriber_name;
        break;
    case 1:
        if ( node->m_subscriber )
            variant = node->m_subscriber->GetPropertyTypeName();
        else
            variant = wxEmptyString;
        break;
    case 2:
        variant = node->m_binding;
        break;
    case 3:
        if ( node->m_observed ) {
            HeeksObj * obj = (HeeksObj*)node->m_observed->GetOwner();
            wxString str = obj->GetTitle ( );
            if ( str.IsEmpty ( ) ) {
                    str = obj->GetTypeString ( );
            }
            variant = str;
        }
        else
            variant = wxEmptyString;
        break;
    case 4:
        if ( node->m_observed )
            variant = node->m_observed->GetName();
        else
            variant = wxEmptyString;
        break;

    default:
        wxLogError ( "PropBindingSubscriberModel::GetValue: wrong column %d", col );
    }
}

bool PropBindingSubscriberModel::SetValue ( const wxVariant &variant,
                                            const wxDataViewItem &item, unsigned int col )
{
    return false;
}

bool PropBindingSubscriberModel::IsEnabled ( const wxDataViewItem &item,
                                             unsigned int col ) const
{
    return true;
}

wxDataViewItem PropBindingSubscriberModel::GetParent ( const wxDataViewItem &item ) const
{
    // the invisible root node has no parent
    if (!item.IsOk())
        return wxDataViewItem(0);

    PropBindingSubscriberModelNode *node = (PropBindingSubscriberModelNode*) item.GetID();

    if (node == m_root)
        return wxDataViewItem(0);

    return wxDataViewItem ( (void*) node->GetParent() );
}

bool PropBindingSubscriberModel::IsContainer ( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return true;
    PropBindingSubscriberModelNode *node = (PropBindingSubscriberModelNode*) item.GetID();
    return ( node->m_container );
}

unsigned int PropBindingSubscriberModel::GetChildren ( const wxDataViewItem &parent,
                                                       wxDataViewItemArray &array ) const
{
    PropBindingSubscriberModelNode *node = (PropBindingSubscriberModelNode*) parent.GetID();
    if (!node)
    {
        array.Add( wxDataViewItem( (void*) m_root ) );
        return 1;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++)
    {
        PropBindingSubscriberModelNode *child = node->GetChildren().Item ( pos );
        array.Add( wxDataViewItem( (void*) child ) );
    }

    return count;
}

PropBindingObservedModelNode::PropBindingObservedModelNode ( PropBindingObservedModelNode* parent,
                                                             const wxString &observed_name,
                                                             Property * observed_prop,
                                                             bool is_container )
 : m_parent ( parent ),
   m_observed_name ( observed_name ),
   m_observed ( observed_prop ),
   m_container ( is_container )
{
}

PropBindingObservedModelNode::~PropBindingObservedModelNode ( )
{
    // free all our children nodes
    size_t count = m_children.GetCount();
    for (size_t i = 0; i < count; i++) {
        PropBindingObservedModelNode *child = m_children[i];
        delete child;
    }
}


PropBindingObservedModel::PropBindingObservedModel ( HeeksObj *obj )
{
    wxString str = obj->GetTitle ( );
    if ( str.IsEmpty ( ) ) {
            str = obj->GetTypeString ( );
    }

    m_root = new PropBindingObservedModelNode ( NULL, str, NULL, true );
    for ( DomainObjectIterator it = obj->begin ( ); it != obj->end ( ); it++ )
    {
        Property * prop = *it;
        if ( prop->IsBindable ( ) ) {
            m_root->Append ( new PropBindingObservedModelNode ( m_root, prop->GetName ( ), prop, prop->IsContainer ( ) ) );
        }
    }
}

PropBindingObservedModel::~PropBindingObservedModel ( )
{
    delete m_root;
}

void PropBindingObservedModel::GetValue ( wxVariant &variant,
                                          const wxDataViewItem &item, unsigned int col ) const
{
    wxASSERT(item.IsOk());

    PropBindingObservedModelNode *node = (PropBindingObservedModelNode*) item.GetID();
    switch (col) {
    case 0:
        variant = node->m_observed_name;
        break;
    case 1:
        if ( node->m_observed )
            variant = node->m_observed->GetPropertyTypeName();
        else
            variant = wxEmptyString;
        break;

    default:
        wxLogError ( "PropBindingObservedModel::GetValue: wrong column %d", col );
    }
}

bool PropBindingObservedModel::SetValue ( const wxVariant &variant,
                                          const wxDataViewItem &item, unsigned int col )
{
    return false;
}

bool PropBindingObservedModel::IsEnabled ( const wxDataViewItem &item,
                                           unsigned int col ) const
{
    return true;
}

wxDataViewItem PropBindingObservedModel::GetParent ( const wxDataViewItem &item ) const
{
    // the invisible root node has no parent
    if (!item.IsOk())
        return wxDataViewItem(0);

    PropBindingObservedModelNode *node = (PropBindingObservedModelNode*) item.GetID();

    if (node == m_root)
        return wxDataViewItem(0);

    return wxDataViewItem ( (void*) node->GetParent() );
}

bool PropBindingObservedModel::IsContainer ( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return true;
    PropBindingObservedModelNode *node = (PropBindingObservedModelNode*) item.GetID();
    return ( node->m_container );
}

unsigned int PropBindingObservedModel::GetChildren ( const wxDataViewItem &parent,
                                                     wxDataViewItemArray &array ) const
{
    PropBindingObservedModelNode *node = (PropBindingObservedModelNode*) parent.GetID();
    if (!node)
    {
        array.Add ( wxDataViewItem( (void*) m_root ) );
        return 1;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++)
    {
        PropBindingObservedModelNode *child = node->GetChildren().Item ( pos );
        array.Add ( wxDataViewItem( (void*) child ) );
    }

    return count;
}

PropBindingDialog::PropBindingDialog ( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
 : wxDialog ( parent, id, title, pos, size, style),
   m_subscriber_model ( NULL ),
   m_observed_model ( NULL )
{
    this->SetSize ( wxSize ( 845, 480 ) );

    wxFlexGridSizer* topSizer = new wxFlexGridSizer ( 4, 1, 0, 0 );
    topSizer->AddGrowableCol ( 0 );
    topSizer->AddGrowableRow ( 0 );

    wxFlexGridSizer* gridSizer = new wxFlexGridSizer ( 1, 2, 0, 0 );
    gridSizer->AddGrowableCol ( 0, 70 );
    gridSizer->AddGrowableCol ( 1, 30 );
    gridSizer->AddGrowableRow ( 0 );

    prop1 = new wxDataViewCtrl ( this, wxID_ANY );
    gridSizer->Add( prop1, 0, wxALL | wxEXPAND, 5 );

    wxDataViewColumn* column;
    column = new wxDataViewColumn ( "subscriber", new wxDataViewTextRenderer(), 0, 120, wxALIGN_LEFT,
                               wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    prop1->AppendColumn ( column );

    column = new wxDataViewColumn ( "type", new wxDataViewTextRenderer(), 1, 100, wxALIGN_LEFT,
                               wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    prop1->AppendColumn ( column );

    column = new wxDataViewColumn ( "binding", new wxDataViewTextRenderer(), 2, 80, wxALIGN_LEFT,
                               wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    prop1->AppendColumn ( column );

    column = new wxDataViewColumn ( "observed", new wxDataViewTextRenderer(), 3, 120, wxALIGN_LEFT,
                               wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    prop1->AppendColumn ( column );

    column = new wxDataViewColumn ( "property", new wxDataViewTextRenderer(), 4, 120, wxALIGN_LEFT,
                               wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    prop1->AppendColumn ( column );

    prop2 = new wxDataViewCtrl ( this, wxID_ANY );
    gridSizer->Add ( prop2, 0, wxEXPAND | wxALL, 5 );

    column = new wxDataViewColumn ( "observed", new wxDataViewTextRenderer(), 0, 120, wxALIGN_LEFT,
                               wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    prop2->AppendColumn ( column );

    column = new wxDataViewColumn ( "type", new wxDataViewTextRenderer(), 1, 100, wxALIGN_LEFT,
                               wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );
    prop2->AppendColumn ( column );

    topSizer->Add ( gridSizer, 1, wxEXPAND | wxALL, 5 );

    wxStaticBoxSizer *actionSizer = new wxStaticBoxSizer ( wxHORIZONTAL, this );
    m_buttonBind = new wxButton ( actionSizer->GetStaticBox(), ID_BIND, _("Bind") );
    actionSizer->Add ( m_buttonBind, 0, wxALL, 5 );

    m_buttonUnbind = new wxButton ( actionSizer->GetStaticBox(), ID_UNBIND, _("Unbind") );
    actionSizer->Add ( m_buttonUnbind, 0, wxALL, 5 );
    actionSizer->Insert ( 0, 260, 0 );
    actionSizer->Insert ( 1, 50, 0 );
    topSizer->Add ( actionSizer, 1, wxEXPAND | wxALL, 5 );

    m_staticline1 = new wxStaticLine ( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    topSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

    m_buttonSizer = new wxStdDialogButtonSizer ( );
    m_buttonOK = new wxButton ( this, wxID_OK );
    m_buttonSizer->AddButton ( m_buttonOK );
    m_buttonSizer->Realize ( );
    m_buttonSizer->SetMinSize ( wxSize ( -1, 40 ) );
    topSizer->Add ( m_buttonSizer, 1, wxEXPAND, 5 );

    this->SetSizer ( topSizer );
    this->Layout ( );

    this->Centre( wxBOTH );
}

PropBindingDialog::~PropBindingDialog()
{
    if ( m_subscriber_model ) {
        m_subscriber_model->DecRef();
    }
    if ( m_observed_model ) {
        m_observed_model->DecRef();
    }
}

void PropBindingDialog::ExpandSubscriberRecursive ( PropBindingSubscriberModel* model,
                                                    wxDataViewItem& item )
{
    prop1->Expand ( item );
    wxDataViewItemArray children;
    int count = model->GetChildren ( item, children );
    for ( int i = 0; i < count; i++ ) {
        wxDataViewItem child = children[i];
        ExpandSubscriberRecursive ( model, child );
    }
}

void PropBindingDialog::AddSubscriberModel ( PropBindingSubscriberModel* model )
{
    m_subscriber_model = model;
    prop1->AssociateModel ( model );
    wxDataViewItem root ( NULL );
    ExpandSubscriberRecursive ( model, root );
    wxGetApp().RegisterObserver ( this );
}

void PropBindingDialog::ExpandObservedRecursive ( PropBindingObservedModel* model,
                                                  wxDataViewItem& item )
{
    prop2->Expand ( item );
    wxDataViewItemArray children;
    int count = model->GetChildren ( item, children );
    for ( int i = 0; i < count; i++ ) {
        wxDataViewItem child = children[i];
        ExpandObservedRecursive ( model, child );
    }
}

void PropBindingDialog::AddObservedModel ( PropBindingObservedModel* model )
{
    m_observed_model = model;
    prop2->AssociateModel ( model );
    wxDataViewItem root ( NULL );
    ExpandObservedRecursive ( model, root );
}

void PropBindingDialog::OnBindButton ( wxCommandEvent& event )
{
    wxDataViewItem selected1 = prop1->GetSelection ( );
    if ( !selected1.IsOk ( ) )
    {
        return;
    }
    PropBindingSubscriberModelNode* subscriber = (PropBindingSubscriberModelNode*) selected1.GetID ( );
    wxDataViewItem selected2 = prop2->GetSelection ( );
    if ( !selected2.IsOk ( ) )
    {
        return;
    }
    PropBindingObservedModelNode* observed = (PropBindingObservedModelNode*) selected2.GetID ( );
    new EqualityBinding ( subscriber->m_subscriber, observed->m_observed );

    // At this point, the subscriber may be a completely new object.

    subscriber->UpdateNode ( );
    m_subscriber_model->ItemChanged ( selected1 );
}

void PropBindingDialog::OnUnbindButton ( wxCommandEvent& event )
{
}

void PropBindingDialog::WhenMarkedListChanges ( bool selection_cleared,
                                                const std::list<HeeksObj*>* added_list,
                                                const std::list<HeeksObj*>* removed_list )
{
    if ( !added_list || added_list->empty ( ) ) {
        return;
    }
    HeeksObj* observed = added_list->back ( );
    PropBindingObservedModel* observed_model = new PropBindingObservedModel ( observed );
    this->AddObservedModel ( observed_model );
}

