#pragma once
#include "PointContainer_.h"

void exportRegion( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Region class
class Region: public PointContainer
{
public:
    //! Constructor
    Region();

    // Destructor
    virtual ~Region() {}

protected:
    //! Check if object is inside region
    bool is_inside( const GameObject & obj ) const;

    //! Update region
    virtual void update( float dt );

    //! Check if point is inside region
    bool is_point_inside( const D3DXVECTOR2 & p ) const;
    
    // Check if point is inside region (in local coordinates)
    bool is_local_point_inside( const D3DXVECTOR2 & p ) const;

    // Check if aabox is intersected
    bool is_box_intersect( const Region & rgn ) const;

    // Friends
    friend class GameObject;
    friend class PathFindRegion;

private:
    friend void ::exportRegion( void );
};

}
