#pragma once

//! Assertion macro
#define ASSERT(expr)

//! Assertion macro with expression evaluation 
//! regardless of environment
#define VERIFY(expr) (expr);

//! Ogg function call with error check.
#define ASSERT_OGG(call) \
    { if (0 != (call)) throw Exception(#call); }
    
//! Ogg function call with error check (extended).
#define ASSERT_OGG_EXT(call, bad_value) \
    { if (bad_value == (call)) throw Exception(#call); }

//! Exception handling macro
#define TRY(expression) expression;
/*                                       \
    try { expression; }                                        \
    catch (PythonException_ &)                                 \
    { throw; }                                                 \
    catch (...)                                                \
    {                                                          \
        Log log;                                               \
        log->error(boost::str(boost::format(                   \
            "Exception caught in file \"%1%\", "               \
            "line %2%. See below for details.") %              \
            Exception_::strip_path(__FILE__) % __LINE__ ));    \
        throw;                                                 \
    }
*/
//! Macro for nothrow ( throw() ) declaration
/** \code
  *   void a() nothrow();
  * \endcode
  */
#define nothrow throw

//! Base class for all exceptions
/** All exception classes end with _ suffix
  * because their names without this ending are
  * reserved for exception constructor macros
  * with __FILE__ and __LINE__ as parameters.
  */
class Exception_: public std::exception
{
public:
    //! Exception constructor with exception location
    /** @param info      individual exception description
      * @param filename  file where exception was thrown
      * @param line      line of code */
    Exception_( const std::string & info, 
                const std::string & filename, int line );
    //! Exception description string
    virtual const char * what() const { return description.c_str(); }

    //! Strip path prefix from filename
    /** Function to strip full path to source files from
    * file names to get relative path (much shorter!)
    * @param  filename  file name to get relative path to */
    static std::string strip_path( const std::string & filename );

protected:
    std::string description;
};
//! Macro for convenient exception initialization
/** Example:
  *  @code
  *    throw Exception("Description");
  *    // This will be expanded to:
  *    // throw Exception_("Description", __FILE__, __LINE__);
  *  @endcode
  */
#define Exception(info) Exception_(info, __FILE__, __LINE__)

//! Macro for exception class header declaration
#define EXCEPTION_CLASS(name)                                 \
    class name ## _: public Exception_                        \
    {                                                         \
    public:                                                   \
        name ## _( const std::string & info,                  \
                   const std::string & filename, int line );  \
    }

//! Exception during attempt to load unexisting file
/** Constructor takes filename as info */
EXCEPTION_CLASS(FileNotExistException);
#define FileNotExistException(info) \
    FileNotExistException_(info, __FILE__, __LINE__)

#undef EXCEPTION_CLASS

//! Python exception
/** This exception should be used only for catching boost::python 
  * errors and couldn't be thrown. */
class PythonException_: public Exception_
{
public:
    PythonException_( const std::string & filename, int line );
    virtual ~PythonException_() = 0 {}
};
