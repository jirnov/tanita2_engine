#include <precompiled.h>
#include "Region_.h"
#include "AxisAlignedBox.h"

void exportRegion( void )
{
    using namespace bp;
    using namespace ingame;

    typedef Wrapper<Region> RegionWrap;
    class_<RegionWrap, bases<PointContainer> >("Region")
        .def("update",       &Region::update,  &RegionWrap::default_update)
        
        .def("is_inside",    &Region::is_inside)
        .def("is_box_intersect", &Region::is_box_intersect)
        .def("is_point_inside", &Region::is_point_inside)
        ;
}

using namespace ingame;

// Constructor
Region::Region()
{
}

// Check if object is inside region
bool Region::is_inside( const GameObject & obj ) const
{
    D3DXVECTOR4 v_tmp;
    D3DXVec2Transform(&v_tmp, &obj.get_absolute_position(), &transformation_inv);
    return is_local_point_inside(D3DXVECTOR2(v_tmp.x, v_tmp.y));
}

// Updating region
void Region::update( float /* dt */ )
{
    begin_update(); // We should remember transformation matrix
                    // for future is_inside checks
    end_update();
}

// Check if point is inside region (in local coordinates)
bool Region::is_local_point_inside( const D3DXVECTOR2 & v ) const
{
    if (getAABox().isInside(v))
    {
        // Intersection count (odd if inside)
        int count = 0;
        const D3DXVECTOR2 v_tmp = D3DXVECTOR2(floorf(v.x)+0.5f, floorf(v.y)+0.5f);
    
        // Finding intersection count
        for (UINT i = 0; i < points.size(); ++i)
            // Checking for intersection with vertical line
            if (check_vray_segment_intersection(v_tmp, points[i], points[(i + 1) % points.size()]))
                count++;
        return count & 1;
    }
    return false;
}

// Check if point (in absolute coordinates) inside region
bool Region::is_point_inside( const D3DXVECTOR2 & p ) const
{
    D3DXVECTOR4 v_tmp;
    D3DXVec2Transform(&v_tmp, &p, &transformation_inv);
    return is_local_point_inside(D3DXVECTOR2(v_tmp.x, v_tmp.y));
}

bool Region::is_box_intersect( const Region & rgn ) const
{
    AxisAlignedBox boxA = AxisAlignedBox(getAABox()).transform(transformation);
    AxisAlignedBox boxB = AxisAlignedBox(rgn.getAABox()).transform(rgn.transformation);
    return boxA.isIntersect(boxB);
}
