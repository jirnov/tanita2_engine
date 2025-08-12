#pragma once

#include "tanita2_.h"
#include <boost/python.hpp>

//! Namespace for python-related definition
namespace python
{
//! Singleton template with subscript operator
template<class T> class MapSingleton: public utility::singleton<T>
{
public:
    //! Subscript operator
    /** @param  key  object name */
    inline bp::object operator []( const std::string & key )
        { TRY(return (*(utility::singleton<T>::operator ->()))[key];) }
};

//! Python script engine class
class PythonInstance
{
public:
    //! Initializes python interpreter
    PythonInstance();
    //! Python interpreter cleanup
    ~PythonInstance();
    
    //! Import python module
    /** @param  path  module path to load */
    void import( const std::string & path );
    
    //! Run arbitrary python code string
    /** @param  code  multi line python code string to run in global namespace */
    bp::object run_string( const std::string & code );
    
    //! Global python namespace access operator
    /** \param  name  name of global object to access */
    inline bp::object operator []( const std::string & name )
        { return main_namespace[name]; }
    
    //! Prints python traceback to user
    static void traceback();
    
    //! Check if object is callable
    inline bool is_callable( bp::object & o ) { return callable(o); }
    
    //! Get object attribute
    inline bp::object getattr( bp::object & o, const std::string & s )
        { return getattr_func(o, bp::str(s), bp::object()); }

protected:
    //! Global python namespace
    bp::object main_namespace;
    
    //! Callable() python function
    bp::object callable;
    
    //! Getattr() python function
    bp::object getattr_func;
};
//! Python singleton definition
typedef MapSingleton<PythonInstance> Python;

} // End of python namespace
