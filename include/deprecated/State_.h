#pragma once

// Forward declaration
void exportState( void );

//! Namespace for game object type declarations
namespace ingame
{

class GameObject;
class AnimatedObject;

//! GameObject state class
class State
{
public:
    //! Constructor
    inline State() {}
    //! Constructor
    /** \param  sequence       sequence to be rendered in this state
      * \param  on_enter       function to be called when entering to state
      * \param  on_update      function to be called while being in state
      * \param  on_exit        function to be called when leaving state
      * \param  link           function returning state name or None */
    State( bp::object sequence, bp::object on_enter = bp::object(), 
                  bp::object on_update = bp::object(), bp::object on_exit = bp::object(),
                  bp::object link = bp::object() );
    
    inline bp::object get_on_enter() { return on_enter; }
    inline bp::object get_on_update() { return on_update; }
    inline bp::object get_on_exit() { return on_exit; }
    inline bp::object get_link() { return link; }
    
    void set_on_enter( bp::object );
    void set_on_update(bp::object);
    void set_on_exit(bp::object);
    void set_link(bp::object);
    
protected:
    //! Sequence to be played in this state
    bp::object sequence;

    //! Function to be called on entering to state
    bp::object on_enter;
    //! Function to be called while being in state
    bp::object on_update;
    //! Function to be called on when exiting from state
    bp::object on_exit;
    
    //! Link function object (returns next state name or None)
    bp::object link;

    // Links for fast access to functions
    bp::object weakmeth;
    bp::object emptyfunc;
    
    // Friend
    friend class GameObject;
    friend class AnimatedObject;

private:
    friend void ::exportState( void );
};

}
