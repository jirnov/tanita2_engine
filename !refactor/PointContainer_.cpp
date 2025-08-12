#include <precompiled.h>
#include "PointContainer_.h"
#include "application_.h"

void exportPointContainer( void )
{
    using namespace bp;
    using namespace ingame;

    typedef AbstractWrapper<PointContainer> PointContainerWrap;
    class_<PointContainerWrap, bases<GameObject>, boost::noncopyable>("PointContainer")
        .def("_release",    &PointContainer::release)
        .def("__len__",     &PointContainer::len)
        .def("__getitem__", &PointContainer::getitem)
        .def("__setitem__", &PointContainer::setitem)
        .def("insert",      &PointContainer::insert)
        .def("__delitem__", &PointContainer::erase)
        .def("clear",       &PointContainer::clear)
        .def("push",        &PointContainer::push)
        .def("load",        &PointContainer::load)
        .def("__nonzero__", &PointContainer::__nonzero__)
        .def("__iter__",    range(&PointContainer::points_begin, &PointContainer::points_end))
        ;
}

using namespace ingame;

// Add point to container
void PointContainer::push( const D3DXVECTOR2 & v )
{
    points.push_back(D3DXVECTOR2(floorf(v.x), floorf(v.y)));
    on_points_change();
}

// Insert point at given position
void PointContainer::insert( unsigned int index, const D3DXVECTOR2 & v )
{
    points.insert(points.begin() + index, D3DXVECTOR2(floorf(v.x), floorf(v.y)));
    on_points_change();
}

// Erase point at given position
void PointContainer::erase( unsigned int index )
{
    points.erase(points.begin() + index);
    on_points_change();
}

// Get item at given position
D3DXVECTOR2 PointContainer::getitem( unsigned int index )
{
    return points[index];
}

// Set item at given position
void PointContainer::setitem( unsigned int index, const D3DXVECTOR2 & v )
{
    points[index] = D3DXVECTOR2(floorf(v.x), floorf(v.y));
    on_points_change();
}

// Load region from file
void PointContainer::load( const DirectoryId & path )
{
    ResourceManagerSingleton fm;
    ResourceRef f = fm->loadResource(path.toResourceId());
    
    // Checking signature
    BYTE * data = reinterpret_cast<U8 *>(f->getStream(RESOURCE_STREAM_DEFAULT));
    if (*(DWORD *)data != PointContainer::signature)
        throw Exception("Points file has invalid signature (perhaps, old file format)");
    DWORD count = *((DWORD *)data + 1);
    
    // Reading region points
    D3DXVECTOR2 * ptr = (D3DXVECTOR2 *)(data + 8);
    for (UINT i = 0; i < count; ++i)
        points.push_back(*(ptr + i));
    on_points_change();
}

// Check if vertical ray intersects the segment
bool PointContainer::check_vray_segment_intersection( const D3DXVECTOR2 & ray_point,
                                             const D3DXVECTOR2 & a, 
                                             const D3DXVECTOR2 & b )
{
    float left = a.x, right  = b.x,
          top = a.y,  bottom = b.y;
    if (left > right) left = b.x, right  = a.x;
    if (bottom > top) top  = b.y, bottom = a.y;
    
    // Ray lays outside segment's rectangle
    if (ray_point.x >= right || ray_point.x <= left || ray_point.y >= top)
        return false;
    
    // Ray lays under segment's rectangle
    if (ray_point.y < bottom) return true;
    
    // Intersection point
    float dx = b.x - a.x, dy = b.y - a.y;

    // Comparison error
    const float EPSILON = 0.0001f; 

    // Segment is a vertical line - let's say there's no intersection
    if (-EPSILON < dx && dx < EPSILON) return false;
    
    float y = dy / dx * (ray_point.x - a.x) + a.y;
    if (y > ray_point.y) return true;

    return false;
}

void PointContainer::release()
{
    if (!isok) return;
    GameObject::release();
}
