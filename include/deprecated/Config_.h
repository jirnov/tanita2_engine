#pragma once
#include "Exceptions_.h"

//! Application configuration database
class ConfigInstance
{
public:
    //! Constructor
    /** Opens configuration file, defined by variable
      * ConfigFile and reads settings from it. */
    ConfigInstance();

    //! Access to configuration value
    /** \code
      *   Config cfg;
      *   int config_value = cfg->get<int>("config_value");
      * \endcode
      * \throw Exception if option not available
      * \note Available configuration value types: int, bool, std::string
      */
    template<typename T> T get( const std::string & );
    
    //! Setting configuration value.
    /** \code
      *   Config cfg;
      *   cfg->set<int>("config_value", 10);
      * \endcode
      * \throw Exception if option is readonly
      * \note Available configuration value types: int, bool, std::string
      */
    template<typename T> void set( const std::string &, T );

protected:
    // Configuration database object
    bp::object database;
};
