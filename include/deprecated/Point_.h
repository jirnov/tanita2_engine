#pragma once
#include "GameObject_.h"

void exportPoint( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Point object
class Point : public GameObject
{
public:
    //! Constructor
    Point() {}
    //! Destructor
    virtual ~Point() { release(); }
    void release() {}

    //! Rendering
    virtual void update( float dt );

    //! Calculate bezier path
    vec2 bezier( bp::list & points, float k );

    //! Calculate distance beetwen two points
    float distance( vec2 & point1, vec2 & point2 );

private:
    friend void ::exportPoint( void );
};

}
