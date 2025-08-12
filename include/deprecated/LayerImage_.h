#pragma once
#include "GameObject_.h"

// Forward declaration
void exportLayerImage( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Layer background image object
class LayerImage: public GameObject
{
public:
    LayerImage() {}
    //! Cleanup
    virtual ~LayerImage() { release(); }
    void release();

    //! Rendering image
    virtual void update( float dt );


    static void create_bindings();

protected:
    //! Load image
    void load_image( const DirectoryId & path, bool compressed );

    //! Load image from direct path
    void direct_load_image( const char * path, bool compressed );

    //! Layer image
    graphics::SequenceRef sequence;
    
    // Friend class
    friend class GameObject;

private:
    friend void ::exportLayerImage( void );
};

}
