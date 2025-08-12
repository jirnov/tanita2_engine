#pragma once

// Forward declaration
void exportTanita2Dict( void );

//! Dictionary class with elements placed in creation order
/** Usual associative containers doesn't obey any
  * element placement order. This container behaves alike
  * python::dict type but places elements in order of 
  * element addition. */
class tanita2_dict
{
public:
    //! Dictionary iterator class
    class iterator
    {
    public:
        // Constructor
        inline iterator( tanita2_dict & dict, unsigned int i ) : dict(dict), index(i) {}
        // Pre-increment operator
        inline iterator operator ++() { return iterator(dict, ++index); }
        
        // Inequality test
        inline bool operator !=( const iterator & i ) { return index != i.index; }
        
        // Get key
        inline bp::object & key() { return dict._keys[index]; }
        // Get value
        inline bp::object & value() { return dict._values[index]; }
        
    protected:
        // Dictionary
        tanita2_dict & dict;
        // Current index
        unsigned int index;
    };
    // Friend
    friend class iterator;

    //! Constructor
    inline tanita2_dict() {}
    
    //! Destructor
    ~tanita2_dict() { _keys.clear(); _values.clear(); }

    //! Copy constructor
    explicit inline tanita2_dict( const tanita2_dict & d )
        : _keys(d._keys), _values(d._values) {}
    
    //! Dictionary start iterator
    inline iterator begin() { return iterator(*this, 0); }
    //! Dictionary end iterator
    inline iterator end() { return iterator(*this, _keys.size()); }
    
    //! Dictionary length
    inline unsigned int len() { return _keys.size(); }
    //! Clearing all elements
    inline void clear() { _keys.clear(); _values.clear(); }
    //! Check key existence
    bool has_key( bp::str & index );
    
    //! Getting item from dictionary (__getitem__ method)
    bp::object & getitem( bp::str & index );
    //! Setting item in dictionary (__setitem__ method)
    void setitem( bp::str & index, bp::object & value );
    //! Deleting item in dictionary (__deltitem__ method)
    void delitem( bp::str & index );
    
    //! Inserting item
    void push_front( bp::str & key, bp::object & value );

    //! Element indexing operator
    bp::object & operator []( bp::str & index );
    
protected:
    //! Erase element from dictionary
    void erase( bp::str & index );

    //! Get keys iteration list
    bp::object iterkeys();
    //! Get values iteration list
    bp::object itervalues();
    //! Get key-value pairs iteration list
    bp::object iteritems();
    //! Swap two items (change placement order)
    void swap( bp::str & a1, bp::str & a2 );

protected:
    // Dictionary keys
    std::vector<bp::str> _keys;
    // Dictionary values
    std::vector<bp::object> _values;

private:
    friend void ::exportTanita2Dict( void );
};

// Namespace for game object type declarations
namespace ingame
{

// Wrapper class template for GameObject child
template<class T>
struct Wrapper: T, bp::wrapper<T>
{
    Wrapper() : T(), bp::wrapper<T>() {}
    Wrapper( const T & w ) : T(w), bp::wrapper<T>() {}

    void update( float dt )
    {
        if (bp::override update = this->get_override("update"))
        {
            update(dt);
            return;
        }
        return T::update(dt);
    }

    void default_update( float dt ) { this->T::update(dt); }
};

// Wrapper class template for abstract GameObject child
template<class T>
struct AbstractWrapper: T, bp::wrapper<T>
{
    void update( float dt )
    {
        this->get_override("update")(dt);
    }
};

} // End of ::ingame namespace
