#pragma once
#include "PointContainer_.h"

//Forward declaration
void exportPath( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Path (trajectory) representation
class Path: public PointContainer
{
public:
    //! Constructor
    Path();
    
    virtual ~Path() {}

protected:
    //! Update path
    virtual void update( float dt );
   
    //! Attach object to path
    void attach( bp::object & obj );
    //! Detach object from path
    inline void detach() { attach(bp::object()); }
    
    //! Stop movement/rotation
    inline void stop() { is_playing = false; }
    //! Resume movement/rotation
    inline void play() { is_playing = true; }

    //! Attached object
    bp::object attached_obj;
    
    //! False if attached object moved over path, True if relative to it
    bool relative_movement;
    //! False if attached object aligned over path, True if relative to it
    bool relative_rotation;
    //! True if path affects object position
    bool affect_position;
    //! True if path affects object rotation (baseline orientation)
    bool affect_rotation;
    //! Reversed direction of movement
    bool reversed;
    
    // Включение хака.
    bool enable_hack;
    
    //! Get reversed flag
    inline bool get_reversed() { return reversed; }
    //! Set reversed flag
    void set_reversed( bool value );
    
    //! Stop/resume flag
    bool is_playing;
    
    // Update for non-reversed movement
    void update_nonreversed( float dt );
    // Update for reversed movement
    void update_reversed( float dt );
    
    // Dynamic parameters for path point
    struct PathPointData
    {
        float speed;
        float angle;
        float dist;
    };
    // Data for path points
    std::vector<PathPointData> point_data;
    typedef std::vector<PathPointData>::iterator PathPointDataIter;
    
    // Distance passed since last point
    float passed_dist;
    // Index of last passed point
    int passed_index;
    // Time passed since last point
    float time_passed;
    
    // Key point class
    struct KeyPoint
    {
        // Constructors
        inline KeyPoint() : speed(0), index(0), reached(false) {}
        inline KeyPoint( int index, float speed )
            : speed(speed), index(index), reached(false) {}
        
        // Speed in pixels/sec
        float speed;
        
        // Index of corresponding point
        int index;
        
        // Flag indicating that point was reached
        bool reached;
    };
    
    // Key points container class
    struct KeyPointContainer
    {
        // __getitem__ method
        inline KeyPoint & KeyPointContainer::getitem( bp::str & id )
            { return kp[id]; }
        // __setitem__ method
        inline void KeyPointContainer::setitem( bp::str & id, KeyPoint & value )
            { kp[id] = value; }
        // __delitem__ method
        inline void KeyPointContainer::delitem( bp::str & id )
            { kp.erase(id); }
        
        // Get list of keys
        inline bp::object KeyPointContainer::iterkeys()
        {
            bp::list t;
            for (KPIter i = kp.begin(); i != kp.end(); ++i)
                t.append(i->first);
            return t;
        }

        // Get list of values
        inline bp::object KeyPointContainer::itervalues()
        {
            bp::list t;
            for (KPIter i = kp.begin(); i != kp.end(); ++i)
                t.append(i->second);
            return t;
        }

        // Get list of key-value pairs
        inline bp::object KeyPointContainer::iteritems()
        {
            bp::list t;
            for (KPIter i = kp.begin(); i != kp.end(); ++i)
                t.append(bp::make_tuple(i->first, i->second));
            return t;
        }
        
        // Key check
        bool KeyPointContainer::has_key( bp::str & id )
        {
            for (KPIter i = kp.begin(); i != kp.end(); ++i)
                if (id == i->first) return true;
            return false;
        }
        
        // Length
        inline unsigned int len() { return kp.size(); }
            
        // Path keypoints
        std::map<bp::str, KeyPoint> kp;
        typedef std::map<bp::str, KeyPoint>::iterator KPIter;
    };
    // Key points
    KeyPointContainer key_points;
    
    // Resetting path object if points were changed
    virtual void on_points_change()
        { attached_obj = bp::object(); key_points.kp.clear(); }

    // Friend
    friend class GameObject;
    friend class PathFindRegion;

private:
    friend void ::exportPath( void );
    
};

}
