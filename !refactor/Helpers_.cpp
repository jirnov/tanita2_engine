#include <precompiled.h>
#include "Helpers_.h"

// Create bindings
void exportTanita2Dict( void )
{
    using namespace bp;

    class_<tanita2_dict>("tanita2_dict")
        .def("__len__",     &tanita2_dict::len)
        .def("__getitem__", &tanita2_dict::getitem, return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &tanita2_dict::setitem, with_custodian_and_ward<1,2>())
        .def("__delitem__", &tanita2_dict::delitem)
        .def("clear",       &tanita2_dict::clear)
        .def("has_key",     &tanita2_dict::has_key)
        .def("iterkeys",    &tanita2_dict::iterkeys)
        .def("itervalues",  &tanita2_dict::itervalues)
        .def("iteritems",   &tanita2_dict::iteritems)
        .def("swap",        &tanita2_dict::swap)
        .def("push_front",  &tanita2_dict::push_front)
        ;
}

// Element indexing operator
bp::object & tanita2_dict::operator []( bp::str & index )
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        if (index == _keys[i]) return _values[i];
    throw Exception(boost::str(boost::format("Invalid key: %s") % bp::extract<char *>(index)));
}

// Erase element from dictionary
void tanita2_dict::erase( bp::str & index )
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        if (index == _keys[i])
        {
            _keys.erase(_keys.begin() + i);
            _values.erase(_values.begin() + i);
            return;
        }
    throw Exception(boost::str(boost::format("Invalid key: %s") % bp::extract<char *>(index)));
}

// Key check
bool tanita2_dict::has_key( bp::str & index )
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        if (index == _keys[i]) return true;
    return false;
}

// Getting item from dictionary
bp::object & tanita2_dict::getitem( bp::str & index )
{
    try { return (*this)[index]; }
    catch (Exception_ &)
    {
        PyErr_SetString(PyExc_KeyError,
            boost::str(boost::format("Invalid key: %s") % bp::extract<char *>(index)).c_str());
        bp::throw_error_already_set();
        throw;
    }
}

// Setting item in dictionary
void tanita2_dict::setitem( bp::str & index, bp::object & value )
{
    for (unsigned int i = 0; i < _keys.size(); ++i)
        if (index == _keys[i])
        {
            _values[i] = value;
            return;
        }
    _keys.push_back(index);
    _values.push_back(value);
}

// Deleting item in dictionary
void tanita2_dict::delitem( bp::str & index )
{
    try { erase(index); }
    catch (Exception_ &)
    {
        PyErr_SetString(PyExc_KeyError,
            boost::str(boost::format("Invalid key: %s") % bp::extract<char *>(index)).c_str());
        bp::throw_error_already_set();
        throw;
    }
}

// Get list of keys
bp::object tanita2_dict::iterkeys()
{
    bp::list t;
    for (std::vector<bp::str>::iterator i = _keys.begin(); i != _keys.end(); ++i)
        t.append(*i);
    return t;
}

// Get list of values
bp::object tanita2_dict::itervalues()
{
    bp::list t;
    for (std::vector<bp::object>::iterator i = _values.begin(); i != _values.end(); ++i)
        t.append(*i);
    return t;
}

// Get list of key-value pairs
bp::object tanita2_dict::iteritems()
{
    bp::list t;
    for (unsigned int i = 0; i < _keys.size(); ++i)
        t.append(bp::make_tuple(_keys[i], _values[i]));
    return t;
}

// Swap two items
void tanita2_dict::swap( bp::str & a1, bp::str & a2 )
{
    if (a1 == a2) return;
    
    bp::object * one = NULL, * two = NULL;
    int i_one = 0, i_two = 0;
    for (unsigned int i = 0; i < _keys.size() && (!one || !two); ++i)
    {
        if (a1 == _keys[i])
        {
            one = &_values[i];
            i_one = i;
            continue;
        }
        if (a2 == _keys[i])
        {
            two = &_values[i];
            i_two = i;
        }
    }
    if (!one || !two)
    {
        PyErr_SetString(PyExc_KeyError, "Invalid key(s)");
        bp::throw_error_already_set();
        throw;
    }
    // Swapping values
    bp::object tmp = *one; *one = *two; *two = tmp;
    // Swapping keys
    bp::str tmp1 = _keys[i_one]; _keys[i_one] = _keys[i_two]; _keys[i_two] = tmp1;
}

// Inserting item
void tanita2_dict::push_front( bp::str & key, bp::object & value )
{
    _keys.insert(_keys.begin(), key);
    _values.insert(_values.begin(), value);
}
