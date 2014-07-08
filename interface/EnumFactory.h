
// expansion macro for enum value definition
#define ENUM_VALUE(name,assign,value) name assign,

// expansion macro for enum to string conversion
#define ENUM_CASE(name,assign,value) case name: return _(#name);

// expansion macro for string to enum conversion
#define ENUM_STRCMP(name,assign,value) if (!::wxStrcmp(str, _(#name))) return name;

// expansion macro for display string
#define ENUM_SHORTSTRING(name,assign,value) case name: return value;

/// declare the access function and define enum values
#define DECLARE_ENUM(ENUM_TYPE,ENUM_DEF) \
  enum ENUM_TYPE { \
    ENUM_DEF(ENUM_VALUE) \
  }; \
  const wxChar *AsString(ENUM_TYPE dummy); \
  ENUM_TYPE  Get ## ENUM_TYPE ## FromString(const wxChar *string); \
  const wxChar *GetShortString(ENUM_TYPE dummy); \
  void operator++(ENUM_TYPE& val); \
  void operator++(ENUM_TYPE& val, int);

/// define the access function names
#define DEFINE_ENUM(ENUM_TYPE,ENUM_DEF) \
  const wxChar *AsString(ENUM_TYPE value) \
  { \
    switch(value) \
    { \
      ENUM_DEF(ENUM_CASE) \
      default: return _(""); /* handle input error */ \
    } \
  } \
  ENUM_TYPE  Get ## ENUM_TYPE ## FromString(const wxChar *str) \
  { \
    ENUM_DEF(ENUM_STRCMP) \
    return (ENUM_TYPE)0; /* handle input error */ \
  } \
  const wxChar* GetShortString(ENUM_TYPE value) \
  { \
    switch(value) \
    { \
      ENUM_DEF(ENUM_SHORTSTRING) \
      default: return _(""); /* handle input error */ \
    } \
  } \
  void operator++(ENUM_TYPE& val) \
  { \
    val = ENUM_TYPE(val+1);  \
  } \
  void operator++(ENUM_TYPE& val, int) \
  { \
    val = ENUM_TYPE(val+1);  \
  }

