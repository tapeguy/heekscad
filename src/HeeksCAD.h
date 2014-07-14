// HeeksCAD.h
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

/*! \brief HeeksCAD Program:
 *
 * Perhaps someone smart could say something useful about this */
#pragma once

#include "../interface/HeeksColor.h"
#include "../interface/HeeksConfig.h"
#include "../interface/ObjList.h"
#include "../interface/Plugin.h"
#include "../interface/PropertyList.h"
#include "glfont2.h"
#include "CxfFont.h"
#include "LineArcDrawing.h"
#include "RegularShapesDrawing.h"
#include <memory>

class TransientObject;
class MagDragWindow;
class ViewRotating;
class ViewZooming;
class ViewPanning;
class CInputMode;
class Drawing;
class CSelectMode;
class DigitizeMode;
class MarkedList;
class GripperMode;
class HeeksObj;
class MarkedObject;
class Gripper;
class CViewPoint;
#ifdef USE_UNDO_ENGINE
class UndoEngine;
#endif
class Observer;
class CHeeksFrame;
class CViewport;
class wxDynamicLibrary;
class CoordinateSystem;
class HRuler;
class wxConfigBase;
class wxAuiManager;
class CSketch;
class CAutoSave;


extern wxString HeeksCADType(const int type);

#define MAX_RECENT_FILES 20

enum GraphicsTextMode
{
	GraphicsTextModeNone,
	GraphicsTextModeInputTitle,
	GraphicsTextModeWithHelp
};

enum FileOpenOrImportType
{
	FileOpenOrImportTypeOther,
	FileOpenTypeHeeks,
	FileImportTypeHeeks,
	FileOpenOrImportTypeDxf
};

enum BackgroundMode
{
	BackgroundModeOneColor,
	BackgroundModeTwoColors,
	BackgroundModeTwoColorsLeftToRight,
	BackgroundModeFourColors,
	BackgroundModeSkyDome
};

enum SolidViewMode
{
	SolidViewFacesAndEdges,
	SolidViewEdgesOnly,
	SolidViewFacesOnly,
};

class HeeksCADapp : public wxApp, public ObjList
{
private:
	std::set<Observer*> observers;
#ifdef USE_UNDO_ENGINE
	UndoEngine *history;
#endif
	std::map<HeeksObj*,std::list<HeeksObj*> > m_transient_objects;

	typedef std::map< int, std::list<HeeksObj*> > IdsToObjects_t;
	typedef int GroupId_t;
	typedef std::map< GroupId_t, IdsToObjects_t > UsedIds_t;

	UsedIds_t	used_ids;

	// std::map< int, std::map<int, HeeksObj*> > used_ids; // map of group type ( usually same as object type ) to "map of ID to object"
	std::map< int, int > next_id_map;
	std::map< std::string, HeeksObj*(*)(TiXmlElement* pElem) > xml_read_fn_map;

	void render_screen_text2(const wxChar* str);
	void RenderDatumOrCurrentCoordSys();

	wxLocale m_locale; // locale we'll be using
	bool m_locale_initialised;

	void StartPickObjects(const wxChar* str, const std::set<MarkingFilter>& marking_filter, bool just_one);
	int EndPickObjects();
	HeeksConfig * config;

public:
	HeeksCADapp();
	~HeeksCADapp();

    double m_view_units; // units to display to the user ( but everything is stored as mm ), 1.0 for mm, 25.4 for inches

	// View option properties
	PropertyList view_options;
	PropertyChoice m_rotate_mode;
	PropertyChoice m_graphics_text_mode;
	PropertyCheck m_antialiasing;
	PropertyCheck m_light_push_matrix;
	PropertyCheck mouse_wheel_forward_away; // true for forwards/backwards = zoom out / zoom in, false for reverse
	PropertyCheck reverse_zoom_mode;
	PropertyCheck ctrl_does_rotate; // true - rotate on Ctrl, pan when not Ctrl      false - rotate when not Ctrl, pan when Ctrl
	PropertyCheck m_datum_coords_system_visible;
	PropertyCheck m_datum_coords_system_solid_arrows;
	PropertyDouble m_datum_coords_system_size;
	PropertyCheck m_show_ruler;
	PropertyCheck m_extrude_removes_sketches;  // config file only
	PropertyCheck m_loft_removes_sketches;     // config file only
	PropertyChoice m_background_mode;
#define NUM_BACKGROUND_COLORS 10
	PropertyColor background_color[NUM_BACKGROUND_COLORS];
	PropertyChoice grid_mode;
	PropertyColor face_selection_color;
	PropertyCheck m_perspective;
	PropertyChoice m_tool_icon_size;
	PropertyCheck m_draw_flat;
	PropertyCheck m_allow_opengl_stippling;
	PropertyChoice m_solid_view_mode;
	PropertyCheck m_input_uses_modal_dialog;
	PropertyCheck m_dragging_moves_objects;

	// Digitizing properties
	PropertyList digitizing;
	PropertyCheck digitize_end;
	PropertyCheck digitize_inters;
	PropertyCheck digitize_centre;
	PropertyCheck digitize_midpoint;
	PropertyCheck digitize_nearest;
	PropertyCheck digitize_tangent;
	PropertyCheck digitize_coords;
	PropertyCheck digitize_screen;
	PropertyLength digitizing_grid;
	PropertyLength digitizing_radius; // for ambiguous arcs and circles
	PropertyCheck draw_to_grid;

	// Correlation properties
	PropertyList correlation_properties;
	PropertyDouble m_min_correlation_factor;
	PropertyDouble m_max_scale_threshold;
	PropertyInt m_number_of_sample_points;
	PropertyCheck m_correlate_by_color;

	// Drawing properties
	PropertyList drawing;
	PropertyColor current_color;
	PropertyColor construction_color;
	PropertyLength m_geom_tol;
	PropertyCheck useOldFuse;
	PropertyCheck m_extrude_to_solid;
	PropertyDouble m_revolve_angle;

	PropertyList selection_filter;

	// File properties
	PropertyList file_options;
	PropertyList dxf_options;

	// STL properties
	PropertyList stl_options;
	PropertyDouble m_stl_facet_tolerance;
	PropertyCheck m_stl_save_as_binary;

	PropertyInt m_auto_save_interval;	// In minutes

	// Font properties
	PropertyList font_options;
	PropertyString m_font_paths;	// SemiColon delimited list of directories that hold font files to load.
	PropertyDouble m_word_space_percentage;	// Font
	PropertyDouble m_character_space_percentage; // Font

	// Logging properties
	PropertyList logging_options;

	wxPoint cur_mouse_pos;
	bool m_gl_font_initialized;

	CoordinateSystem *m_current_coordinate_system;
	LineArcDrawing m_line_strip;
	RegularShapesDrawing m_regular_shapes_drawing;

	CInputMode *input_mode_object;
	MagDragWindow *magnification;
	ViewRotating *viewrotating;
	ViewZooming *viewzooming;
	ViewPanning *viewpanning;
	CSelectMode *m_select_mode;
	DigitizeMode *m_digitizing;
	GripperMode* gripper_mode;
	Gripper *drag_gripper;
	gp_Pnt grip_from, grip_to;
	Gripper *cursor_gripper;
	CHeeksFrame *m_frame;
	CViewport *m_current_viewport;
	MarkedList *m_marked_list;
	bool m_doing_rollback;
	wxString m_filepath;
	bool m_untitled;
	std::list<HeeksObj*> m_hidden_for_drag;
	bool m_show_grippers_on_drag;
	std::list<Plugin> m_loaded_libraries;
	std::list< void(*)() > m_on_glCommands_list;
	std::list< wxAuiToolBar* > m_external_toolbars;
	std::list< void(*)() > m_AddToolBars_list;
	std::list<wxWindow*> m_hideable_windows;
	HRuler* m_ruler;
	std::list< wxString > m_recent_files;
	bool m_in_OpenFile;
	bool m_mark_newly_added_objects;
	wxString m_version_number;
	std::list< void(*)(wxSizeEvent&) > m_on_graphics_size_list;
	std::list< void(*)(wxMouseEvent&) > m_lbutton_up_callbacks;
	std::list< void(*)(bool) > m_on_save_callbacks;
	std::list< bool(*)() > m_is_modified_callbacks;
	std::list< void(*)() > m_on_build_texture_callbacks;
	std::list< void(*)(int, int) > m_beforeneworopen_callbacks;
	std::list< void(*)() > m_beforeframedelete_callbacks;
	int m_transform_gl_list;
	gp_Trsf m_drag_matrix;
	bool m_font_created;
	glfont::GLFont m_gl_font;
	bool m_sketch_mode;
	CSketch* m_sketch;
	unsigned int m_font_tex_number;
	bool m_print_scaled_to_page;
	wxPrintData *m_printData;
	wxPageSetupDialogData* m_pageSetupData;
	FileOpenOrImportType m_file_open_or_import_type;
	bool m_inPaste;
	double* m_file_open_matrix;
	bool m_no_creation_mode; // set from a plugin, for making an exporter only application

	bool m_property_grid_validation;

	std::auto_ptr<VectorFonts>	m_pVectorFonts;	// QCAD format fonts that have been loaded.
	VectorFont   *m_pVectorFont;	// which font are we using? (NULL indicates the internal (OpenGL) font)

	std::auto_ptr<CAutoSave> m_pAutoSave;

	bool m_isModified;
	bool m_isModifiedValid;

	int m_icon_texture_number;

	typedef void(*FileOpenHandler_t)(const wxChar *path);
	typedef std::map<wxString, FileOpenHandler_t> FileOpenHandlers_t;

	FileOpenHandlers_t  m_fileopen_handlers;

	bool RegisterFileOpenHandler( const std::list<wxString> file_extensions, FileOpenHandler_t );
	bool UnregisterFileOpenHandler( void (*fileopen_handler)(const wxChar *path) );

	typedef void(*UnitsChangedHandler_t)(const double value);
	typedef std::list<UnitsChangedHandler_t> UnitsChangedHandlers_t;

	UnitsChangedHandlers_t m_units_changed_handlers;

	void RegisterUnitsChangeHandler( UnitsChangedHandler_t );
	void UnregisterUnitsChangeHandler( UnitsChangedHandler_t );

	typedef wxString(*HeeksTypesConverter_t)(const int type);
	typedef std::list<HeeksTypesConverter_t> HeeksTypesConverters_t;

	HeeksTypesConverters_t m_heeks_types_converters;

	void RegisterHeeksTypesConverter( HeeksTypesConverter_t );
	void UnregisterHeeksTypesConverter( HeeksTypesConverter_t );

	wxString m_alternative_open_wild_card_string;

	//WxApp override
	int OnRun();
	bool OnExceptionInMainLoop();
    bool OnInit();
    int OnExit();

	// HeeksObj's virtual functions
	void GetBox(CBox &box);
	void glCommands(bool select, bool marked, bool no_color);
	bool CanAdd(HeeksObj* object){return true;}
	int GetType()const{return DocumentType;}

	HeeksConfig& GetConfig();
	void WriteConfig();
	void CreateLights(void);
	void DestroyLights(void);
	void FindMarkedObject(const wxPoint &point, MarkedObject* marked_object);
	void SetInputMode(CInputMode *i);
	void Repaint(bool soon = false);
	void RecalculateGLLists();
	void SetLikeNewFile(void);
	bool IsModified(void);
	void ClearHistory(void);
	void glCommandsAll(const CViewPoint &view_point);
	double GetPixelScale(void);
	void DoMoveOrCopyDropDownMenu(wxWindow *wnd, const wxPoint &point, MarkedObject* marked_object, HeeksObj* paste_into, HeeksObj* paste_before);
	void GetDropDownTools(std::list<Tool*> &f_list, const wxPoint &point, MarkedObject* marked_object, bool dont_use_point_for_functions, bool control_pressed);
	void DoDropDownMenu(wxWindow *wnd, const wxPoint &point, MarkedObject* marked_object, bool dont_use_point_for_functions, bool control_pressed);
	void GenerateIntersectionMenuOptions( std::list<Tool*> &f_list );
	void on_menu_event(wxCommandEvent& event);
	void DoToolUndoably(Tool *);
	void Undo(void);
	void Redo(void);
	void WentTransient(HeeksObj* obj, TransientObject* tobj);
	void ClearTransients();
	std::map<HeeksObj*,std::list<HeeksObj*> >& GetTransients();
	bool Add(HeeksObj* object, HeeksObj* prev_object);
	void Remove(HeeksObj* object);
	void Remove(std::list<HeeksObj*> objects);
	void Transform(std::list<HeeksObj*> objects, double *m);
	void Reset();
	HeeksObj* ReadXMLElement(TiXmlElement* pElem);
	void ObjectWriteBaseXML(HeeksObj *object, TiXmlElement *element);
	void ObjectReadBaseXML(HeeksObj *object, TiXmlElement* element);
	void InitializeXMLFunctions();
	void OpenXMLFile(const wxChar *filepath,HeeksObj* paste_into = NULL, HeeksObj* paste_before = NULL);
	static void OpenSVGFile(const wxChar *filepath);
	static void OpenSTLFile(const wxChar *filepath);
	static void OpenDXFFile(const wxChar *filepath);
	static void OpenRS274XFile(const wxChar *filepath);
	bool OpenImageFile(const wxChar *filepath);

	void OnNewButton();
	void OnOpenButton();
	bool OpenFile(const wxChar *filepath, bool import_not_open = false, HeeksObj* paste_into = NULL, HeeksObj* paste_before = NULL, bool retain_filename = true );
	void SaveDXFFile(const wxChar *filepath);
	void SaveSTLFileBinary(const std::list<HeeksObj*>& objects, const wxChar *filepath, double facet_tolerance = -1.0, double* scale = NULL);
	void SaveSTLFileAscii(const std::list<HeeksObj*>& objects, const wxChar *filepath, double facet_tolerance = -1.0, double* scale = NULL);
	void SaveSTLFile(const std::list<HeeksObj*>& objects, const wxChar *filepath, double facet_tolerance = -1.0, double* scale = NULL, bool binary = true);
	void SaveCPPFile(const std::list<HeeksObj*>& objects, const wxChar *filepath, double facet_tolerance = -1.0);
	void SavePyFile(const std::list<HeeksObj*>& objects, const wxChar *filepath, double facet_tolerance = -1.0);
	void SaveXMLFile(const std::list<HeeksObj*>& objects, const wxChar *filepath, bool for_clipboard = false);
	void SaveXMLFile(const wxChar *filepath){SaveXMLFile(m_objects, filepath);}
#ifdef CONSTRAINT_TESTER
    //JT
	 virtual void AuditHeeksObjTree4Constraints(HeeksObj * SketchPtr ,HeeksObj* mom, int level,bool ShowMsgInConsole,bool * ConstraintsAreOk){};
#endif

	bool SaveFile(const wxChar *filepath, bool use_dialog = false, bool update_recent_file_list = true, bool set_app_caption = true);
	void CreateUndoPoint();
	void Changed();
	gp_Trsf GetDrawMatrix(bool get_the_appropriate_orthogonal);
	void GetProperties(std::list<Property *> *list);
	void InitializeProperties();
	void DeleteMarkedItems();
	void glColorEnsuringContrast(const HeeksColor &c);
	void RegisterObserver(Observer* observer);
	void RemoveObserver(Observer* observer);
	void ObserversOnChange(const std::list<HeeksObj*>* added, const std::list<HeeksObj*>* removed, const std::list<HeeksObj*>* modified);
	void ObserversMarkedListChanged(bool selection_cleared, const std::list<HeeksObj*>* added, const std::list<HeeksObj*>* removed);
	void ObserversFreeze();
	void ObserversThaw();
	const wxChar* GetKnownFilesWildCardString(bool open = true)const;
	const wxChar* GetKnownFilesCommaSeparatedList(bool open = true)const;
	void GetTools(MarkedObject* marked_object, std::list<Tool*>& t_list, const wxPoint& point, bool control_pressed);
	void GetTools2(MarkedObject* marked_object, std::list<Tool*>& t_list, const wxPoint& point, bool control_pressed);
	wxString GetExeFolder()const;
	wxString GetResFolder()const;
	wxString GetTmpFolder()const;
	void get_2d_arc_segments(double xs, double ys, double xe, double ye, double xc, double yc, bool dir, bool want_start, double pixels_per_mm, void(*callbackfunc)(const double* xy));
	int PickObjects(const wxChar* str, const std::set<MarkingFilter>& marking_filter = MarkedList::all_filters, bool just_one = false);
	bool PickPosition(const wxChar* str, double* pos, void(*callback)(const double*) = NULL);
	void glSphere(double radius, const double* pos = NULL);
	void OnNewOrOpen(bool open, int res);
	void OnBeforeNewOrOpen(bool open, int res);
	void OnBeforeFrameDelete(void);
	void RegisterHideableWindow(wxWindow* w);
	void RemoveHideableWindow(wxWindow* w);
	void RegisterReadXMLfunction(const char* type_name, HeeksObj*(*read_xml_function)(TiXmlElement* pElem));
	void GetRecentFilesProfileString();
	void WriteRecentFilesProfileString(wxConfigBase &config);
	void InsertRecentFileItem(const wxChar* filepath);
	int CheckForModifiedDoc(); // returns wxCANCEL, if NOT OK to continue with file open etc.
	void SetFrameTitle();
	HeeksObj* GetIDObject(int type, int id);
	std::list<HeeksObj*> GetIDObjects(int type, int id);
	void SetObjectID(HeeksObj* object, int id);
	int GetNextID(int type);
	void RemoveID(HeeksObj* object); // only call this from ObjList::Remove()
	void ResetIDs();
	bool InputInt(const wxChar* prompt, const wxChar* value_name, int &value);
	bool InputDouble(const wxChar* prompt, const wxChar* value_name, double &value);
	bool InputAngleWithPlane(double &angle, double *axis = NULL, double *pos = NULL, int *number_of_copies = NULL, double *axial_shift = NULL);
	bool InputLength(const wxChar* prompt, const wxChar* value_name, double &value);
	void ShowModalOptions();
	void SectioningDialog();
	void RegisterOnGLCommands( void(*callbackfunc)() );
	void RemoveOnGLCommands( void(*callbackfunc)() );
	void RegisterOnGraphicsSize( void(*callbackfunc)(wxSizeEvent&) );
	void RemoveOnGraphicsSize( void(*callbackfunc)(wxSizeEvent&) );
	void RegisterOnMouseFn( void(*callbackfunc)(wxMouseEvent&) );
	void RemoveOnMouseFn( void(*callbackfunc)(wxMouseEvent&) );
	void RegisterOnSaveFn( void(*callbackfunc)(bool) );
	void RegisterIsModifiedFn( bool(*callbackfunc)() );
	void CreateTransformGLList(const std::list<HeeksObj*>& list, bool show_grippers_on_drag);
	void DestroyTransformGLList();
	bool IsPasteReady();
	void EnableBlend();
	void DisableBlend();
	void Paste(HeeksObj* paste_into, HeeksObj* paste_before);
	bool CheckForNOrMore(const std::list<HeeksObj*> &list, int min_num, int type, const wxString& msg, const wxString& caption);
	bool CheckForNOrMore(const std::list<HeeksObj*> &list, int min_num, int type1, int type2, const wxString& msg, const wxString& caption);
	bool CheckForNOrMore(const std::list<HeeksObj*> &list, int min_num, int type1, int type2, int type3, const wxString& msg, const wxString& caption);
	void render_text(const wxChar* str);
	bool get_text_size(const wxChar* str, float* width, float* height);
	void render_screen_text(const wxChar* str1, const wxChar* str2);
	void render_screen_text_at(const wxChar* str1, double scale, double x, double y, double theta);
	void OnInputModeTitleChanged();
	void OnInputModeHelpTextChanged();
	void PlotSetColor(const HeeksColor &c);
	void PlotLine(const double* s, const double* e);
	void PlotArc(const double* s, const double* e, const double* c);
	void InitialiseLocale();
	void create_font();
	CSketch* GetContainer();
	bool EndSketchMode();
	void EnterSketchMode(CSketch* sketch);
	std::auto_ptr<VectorFonts>	& GetAvailableFonts(const bool force_read = false);
	void GetPluginsFromCommandLineParams(std::list<wxString> &plugins);
	void RegisterOnBuildTexture(void(*callbackfunc)());
	void RegisterOnBeforeNewOrOpen(void(*callbackfunc)(int, int));
	void RegisterOnBeforeFrameDelete(void(*callbackfunc)());
	const HeeksColor& CurrentColor() const;
	const HeeksColor& ConstructionColor() const;
	void LogDebug(std::string msg);

	typedef int ObjectType_t;
	typedef int ObjectId_t;
	typedef std::pair< ObjectType_t, ObjectId_t > ObjectReference_t;
	typedef std::map< ObjectReference_t, HeeksObj * > ObjectReferences_t;

	HeeksObj *MergeCommonObjects( ObjectReferences_t & unique_set, HeeksObj *object ) const;

	wxString HeeksType( const int type ) const;

	bool UsesID() { return false; }
	bool UsesColor() { return false; }

//JT
#ifdef CONSTRAINT_TESTER
     bool TestForValidConstraints(){return TestForValidConstraints(m_objects);};//m_objects is protected and visible to class or sub_class
   	 bool TestForValidConstraints(const std::list<HeeksObj*>& objects);

#endif

	unsigned int GetIndex(HeeksObj *object);
	void ReleaseIndex(unsigned int index);
};

void ExitMainLoop();


DECLARE_APP(HeeksCADapp)

