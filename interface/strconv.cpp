// strconv.cpp
// Copyright (c) 2009, Dan Heeks
// This program is released under the BSD license. See the file COPYING for details.

#include <string>
#include <wx/string.h>
#include <vector>

const char* Ttc(const wchar_t* str)
{
    static std::string _str;
    _str.clear();
    wxString _wxstr(str);

    _str.append((const char *) _wxstr.mb_str(wxConvUTF8));
    return(_str.c_str());
}

/**
	Breakup the line of text based on the delimiting characters passed
	in and return a vector of 'words'.
 */
std::vector<wxString> Tokens( const wxString& wxLine, const wxString& wxDelimiters )
{
    std::vector<wxString> tokens;
    wxString line(wxLine);    // non-const copy

    wxString::size_type offset;
    while ((offset = line.find_first_of( wxDelimiters )) != wxNOT_FOUND)
    {
        if (offset > 0)
        {
            tokens.push_back( line.substr(0, offset) );
        }

        line.Remove(0, offset+1);
    }

    if (line.size() > 0)
    {
        tokens.push_back( line );
    }

    return(tokens);

} // End Tokens() method



bool AllNumeric( const wxString wxLine )
{
	if (wxLine.Length() == 0) return(false);

	wxString line( wxLine );	// non-const copy
	wxString::size_type offset;

	for (offset=0; offset<line.size(); offset++)
	{
		if ((((line[offset] >= _T('0')) &&
		     (line[offset] <= _T('9'))) ||
		    (line[offset] == '+') ||
		    (line[offset] == '-') ||
		    (line[offset] == '.')) == false) return(false);
	} // End for
	return(true);
} // End Tokens() method


