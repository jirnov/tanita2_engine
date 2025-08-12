#include <precompiled.h>
#include "Exceptions_.h"

using namespace boost;

// Constructor
Exception_::Exception_( const std::string & info,
                        const std::string & filename, int line )
{
    description = boost::str(format("Exception in file \"%1%\", line %2%. %3%")
                                    % strip_path(filename) % line % info);
}

// Strip path prefix from filename
std::string Exception_::strip_path( const std::string & filename )
{
    using namespace std;
    
    string pattern = "\\code\\tanita2\\", fname = filename;
    use_facet<ctype<char> >(locale()).tolower(&fname[0], &fname[fname.length()-1]);
    
    string::size_type offset = fname.find(pattern);
    if (string::npos == offset) return fname;
    return std::string(fname, offset + pattern.length());
}
/*
// Constructor
WinAPIException_::WinAPIException_( const std::string & info,
                                    const std::string & filename, int line )
    : Exception_("", filename, line)
{
    DWORD error = GetLastError();
    char * message;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL, error, MAKELANGID(LANG_SYSTEM_DEFAULT, 
                  SUBLANG_DEFAULT), (char *)&message, 0, NULL);
    description += boost::str(format("\nMessage: \"%1%\" call failed with"
                                     " the following error (%2$X): %3%")
                                     % info % error % message);
    LocalFree(message);
}

// Constructor
DirectXException_::DirectXException_( const std::string & info, long hr,
                                      const std::string & filename, int line )
    : Exception_("", filename, line)
{
    description += boost::str(format("\nMessage: \"%1%\" call failed with"
                                     " the following error (%2%): %3%")
                                     % info % DXGetErrorString9(hr)
                                     % DXGetErrorDescription9(hr));
}
*/
// Constructor
PythonException_::PythonException_( const std::string & filename, int line )
    : Exception_("", filename, line)
{
    try { PyErr_Print(); }
    catch (...) {}
    description = boost::str(format("Python exception. See traceback for details."));
}

// Constructor
FileNotExistException_::FileNotExistException_( const std::string & info, 
                                                const std::string & filename, int line )
    : Exception_("", filename, line)
{
    description += boost::str(format("File not found: \"%1%\"") % strip_path(info));
}
