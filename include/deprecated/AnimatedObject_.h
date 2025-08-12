#pragma once
#include "GameObject_.h"

// Forward declaration
void exportAnimatedObject( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Object with states and animations
class AnimatedObject: public GameObject
{
protected:
    //! Constructor
    inline AnimatedObject() : is_inside_zregion(false)
        { states.parent = this;}
    
    //! Destructor
    virtual ~AnimatedObject() { release(); }
    void release();

    //! Load large sequence
    void add_large_sequence( bp::str & name, const DirectoryId & path, bp::tuple & frame_numbers,
                             bool compressed );
    //! Load large sequence with sounds
    void add_large_sound_sequence( bp::str & name, const DirectoryId & path, bp::tuple & frame_numbers,
                                   bp::tuple & sounds, bool compressed );
    //! Load video sequence with sounds
    void add_video_sound_sequence( bp::str & name, const DirectoryId & path );

    //! Updating object
    virtual void update( float dt );

    //! Sequences
    tanita2_dict sequences;
    //! States
    objects_dict states;
    
    //! Current state
    bp::str state;
    
    //! Set state
    void set_state( bp::str & new_state );
    //! Get current state
    inline bp::str get_state() { return state; }
    
    //! Flag indicating that object is inside z region
    bool is_inside_zregion;

    // Friend class
    friend class GameObject;

private:
    friend void ::exportAnimatedObject( void );
};

}
