#include <precompiled.h>
#include "Path_.h"
#include "Python_.h"

void exportPath( void )
{
    using namespace bp;
    using namespace ingame;

    class_<Path::KeyPoint>("KeyPoint", init<int, float>())
        .def(init<>())
        .def_readwrite("index",  &Path::KeyPoint::index)
        .def_readwrite("speed",  &Path::KeyPoint::speed)
        
        .def_readonly("reached", &Path::KeyPoint::reached)
        ;

    class_<Path::KeyPointContainer>("KeyPointList")
        .def("__getitem__", &Path::KeyPointContainer::getitem, return_internal_reference<>())
        .def("__setitem__", &Path::KeyPointContainer::setitem, with_custodian_and_ward<1,2>())
        .def("__delitem__", &Path::KeyPointContainer::delitem)
        .def("__len__",     &Path::KeyPointContainer::len)
        
        .def("has_key",     &Path::KeyPointContainer::has_key)
        .def("iterkeys",    &Path::KeyPointContainer::iterkeys)
        .def("itervalues",  &Path::KeyPointContainer::itervalues)
        .def("iteritems",   &Path::KeyPointContainer::iteritems)
        ;

    typedef Wrapper<Path> PathWrap;
    class_<PathWrap, bases<PointContainer> >("Path")
        .def("update", &Path::update,  &PathWrap::default_update)
        .def("attach", &Path::attach)
        .def("detach", &Path::detach)
        
        .def("stop", &Path::stop)
        .def("play", &Path::play)
        
        .def_readonly("is_playing", &Path::is_playing)
        .def_readonly("key_points", &Path::key_points)
        
        .def_readwrite("enable_hack",       &Path::enable_hack)
        .def_readwrite("affect_position",   &Path::affect_position)
        .def_readwrite("affect_rotation",   &Path::affect_rotation)
        .def_readwrite("relative_movement", &Path::relative_movement)
        .def_readwrite("relative_rotation", &Path::relative_rotation)

        .add_property("reversed", &Path::get_reversed, &Path::set_reversed)
        ;
}

using namespace ingame;

// Constructor
Path::Path() : affect_position(true),   affect_rotation(true), 
               relative_movement(true), relative_rotation(true),
               reversed(false), is_playing(false), enable_hack(false)
{
}

// Updating path
void Path::update( float dt )
{
    // Rendering sequence
    
    // Resetting 'reached' flag for all points
    if (is_playing && bp::object() == attached_obj)
    {
        for (KeyPointContainer::KPIter i = key_points.kp.begin(); key_points.kp.end() != i; ++i)
            i->second.reached = false;
        is_playing = false;
    }

    // Check if path really should be updated
    if (!is_playing || bp::object() == attached_obj ||
        (!affect_position && !affect_rotation))
        return;
    
    begin_update();
    
    void (Path::*update_proc)( float dt ) = reversed ? &Path::update_reversed : &Path::update_nonreversed;
    //(this->*update_proc)(dt);
    while (dt > 0)
    {
        (this->*update_proc)(std::min<float>(dt, 0.5f));
        dt -= 0.5;
    }
    end_update();
}

// Update for non-reversed movement
void Path::update_nonreversed( float dt )
{
    // Detaching object if reached the end
    if (passed_index >= (int)points.size()-1)
    {
        attached_obj = bp::object();
        return;
    }
    
    // Resetting previously reached flags
    for (KeyPointContainer::KPIter i = key_points.kp.begin(); key_points.kp.end() != i; ++i)
        if (i->second.index <= passed_index)
            i->second.reached = false;
    
    // Moving object
    if (affect_position)
    {
        // Old values
        int old_passed_index = passed_index;
        float old_passed_dist = passed_dist;
        
        do
        {
            // Calculating acceleration
            const float dv = point_data[passed_index + 1].speed - point_data[passed_index].speed,
                        v0 = point_data[passed_index].speed;
            const float a = (2 * v0 * dv + dv * dv) / (2 * point_data[passed_index+1].dist);
            
            // Runge-Kutta integration coefficients
            const float k1 = (point_data[passed_index].speed + a * time_passed) * dt;
            const float k2 = (point_data[passed_index].speed + k1 / 2 + (a * (time_passed + dt / 2))) * dt;
            const float k3 = (point_data[passed_index].speed + k2 / 2 + (a * (time_passed + dt / 2))) * dt;
            const float k4 = (point_data[passed_index].speed + k3 + (a * (time_passed + dt))) * dt;
            
            // Integration
            passed_dist += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
            time_passed += dt;
            
            // Break if we have not moved beyond next point
            if (passed_dist < point_data[passed_index+1].dist)
                break;
            
            // Останавливаемся, когда достигаем ключевой точки.
            // Хак для персонажей.
            //
            passed_dist -= point_data[passed_index+1].dist;
            
            if (enable_hack)
                passed_dist = 0;
            time_passed = 0;
            
            // Updating key point reached flags
            ++passed_index;
            for (KeyPointContainer::KPIter i = key_points.kp.begin(); key_points.kp.end() != i; ++i)
                if (i->second.index == passed_index)
                    i->second.reached = true;
            
            // Detaching object if reached the end
            if (passed_index >= (int)points.size()-1)
            {
                attached_obj = bp::object();
                return;
            }
            
            if (enable_hack)
                break;
        } while (true);
        
        // Calculating new position
        float t = passed_dist / point_data[passed_index+1].dist;
        D3DXVECTOR2 pos = points[passed_index + 1] * t + points[passed_index] * (1 - t);
        
        // Moving attached object
        GameObject & attached = bp::extract<GameObject &>(attached_obj());
        if (relative_movement)
        {
            float old_t = old_passed_dist / point_data[old_passed_index+1].dist;
            D3DXVECTOR2 old_pos = points[old_passed_index + 1] * old_t +
                                  points[old_passed_index] * (1 - old_t);
            
            attached.position += pos - old_pos;
        }
        else
        {
            D3DXVECTOR2 dp = get_absolute_position() + pos - attached.get_absolute_position();
            attached.position += dp;
        }
    }
}

// Update for reversed movement
void Path::update_reversed( float dt )
{
    // Detaching object if reached the end
    if (passed_index <= 0)
    {
        attached_obj = bp::object();
        return;
    }
    
    // Resetting previously reached flags
    for (KeyPointContainer::KPIter i = key_points.kp.begin(); key_points.kp.end() != i; ++i)
        if (i->second.index >= passed_index)
            i->second.reached = false;
    
    // Moving object
    if (affect_position)
    {
        // Old values
        int old_passed_index = passed_index;
        float old_passed_dist = passed_dist;
        
        do
        {
            // Calculating acceleration
            const float dv = point_data[passed_index-1].speed - point_data[passed_index].speed,
                        v0 = point_data[passed_index].speed;
            const float a = (2 * v0 * dv + dv * dv) / (2 * point_data[passed_index].dist);
            
            // Runge-Kutta integration coefficients
            const float k1 = (point_data[passed_index].speed + a * time_passed) * dt;
            const float k2 = (point_data[passed_index].speed + k1 / 2 + (a * (time_passed + dt / 2))) * dt;
            const float k3 = (point_data[passed_index].speed + k2 / 2 + (a * (time_passed + dt / 2))) * dt;
            const float k4 = (point_data[passed_index].speed + k3 + (a * (time_passed + dt))) * dt;
            
            // Integration
            passed_dist -= (k1 + 2 * k2 + 2 * k3 + k4) / 6;
            time_passed += dt;
            
            // Break if we have not moved beyond next point
            if (passed_dist > 0)
                break;
            
            // Moving to next pair of points
            --passed_index;
            passed_dist += point_data[passed_index].dist;
            time_passed = 0;
            
            // Updating key point reached flags
            for (KeyPointContainer::KPIter i = key_points.kp.begin(); key_points.kp.end() != i; ++i)
                if (i->second.index == passed_index)
                    i->second.reached = true;
            
            // Detaching object if reached the end
            if (passed_index <= 0)
            {
                attached_obj = bp::object();
                return;
            }
        } while (true);
        
        // Calculating new position
        float t = 1 - passed_dist / point_data[passed_index].dist;
        D3DXVECTOR2 pos = points[passed_index - 1] * t + points[passed_index] * (1 - t);
        
        // Moving attached object
        GameObject & attached = bp::extract<GameObject &>(attached_obj());
        if (relative_movement)
        {
            float old_t = 1 - old_passed_dist / point_data[old_passed_index].dist;
            D3DXVECTOR2 old_pos = points[old_passed_index - 1] * old_t +
                                  points[old_passed_index] * (1 - old_t);
            
            attached.position += pos - old_pos;
        }
        else
        {
            D3DXVECTOR2 dp = get_absolute_position() + pos - attached.get_absolute_position();
            attached.position += dp;
        }
    }
}

// Set reversed flag
void Path::set_reversed( bool value )
{
    reversed = value;
    passed_index += value ? 1 : -1;
}

// Attaching object to path
void Path::attach( bp::object & obj )
{
    // Checking arguments
    if (points.empty() || bp::object() == obj)
    {
        attached_obj = bp::object();
        return;
    }
    // Updating points data
    const UINT points_size = points.size();
    point_data.resize(points_size);
    
    // Attaching object
    python::Python py;
    attached_obj = py["weakref"].attr("ref")(obj);
    
    // Calculating path data for every path point
    int start_index = 0;
    float previous_speed = 0.0f;
    for (KeyPointContainer::KPIter kp = key_points.kp.begin(); key_points.kp.end() != kp; ++kp)
    {
        // Starting point is reached by definition
        kp->second.reached = ((reversed ? points_size-1 : 0) == kp->second.index);
        
        float dist = 0;
        ASSERT(kp->second.index < (int)points_size);
        
        // Calculating distance between keypoints
        for (int i = start_index; i < kp->second.index + 1; ++i)
            if (0 == i)
                point_data[i].dist = 0;
            else
            {
                float dx = points[i].x - points[i-1].x,
                      dy = points[i].y - points[i-1].y;
                point_data[i].dist = sqrtf(dx * dx + dy * dy);
                dist += point_data[i].dist;
            }

        // Calculating speed and rotation angles for points
        if (0.001 < dist)
        {
            float dist_passed = 0;
            for (int i = start_index; i < kp->second.index; ++i)
            {
                dist_passed += point_data[i].dist;
                
                float t = (start_index == i) ? 0 : dist_passed / dist;
                point_data[i].speed = kp->second.speed * t + previous_speed * (1 - t);
            }
        }
        previous_speed = kp->second.speed;
        start_index = kp->second.index;
    }
    for (UINT i = start_index; i < points_size; ++i)
    {
        if (0 == i)
            point_data[i].dist = 0;
        else
        {
            float dx = points[i].x - points[i-1].x,
                  dy = points[i].y - points[i-1].y;
            point_data[i].dist = sqrtf(dx * dx + dy * dy);
        }
        point_data[i].speed = previous_speed;
    }
    
    // Enabling path handling
    is_playing = true;
    
    // Misc initializations
    passed_index = 0;
    passed_dist = 0;
    time_passed = 0;
    if (reversed)
    {
        passed_index = points_size - 1;
        passed_dist = point_data[passed_index].dist;
        
        GameObject & attached = bp::extract<GameObject &>(attached_obj());
        
        if (relative_movement)
            attached.position += points[passed_index] - points[0];
        else
        {
            D3DXVECTOR2 dp = get_absolute_position() + points[passed_index] - 
                             attached.get_absolute_position();
            attached.position += dp;
        }
    }
}
