#pragma once
#include "GameObject_.h"
#include "AxisAlignedBox.h"

void exportPointContainer( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Abstract base class for path and region
class PointContainer: public GameObject
{
public:
    //! Constructor
    inline PointContainer()
        {}

    //! Destructor
    virtual ~PointContainer() = 0 { release(); }
    void release();
    
protected:
    static bool check_vray_segment_intersection( const D3DXVECTOR2 & ray_point,
                                                 const D3DXVECTOR2 & a, 
                                                 const D3DXVECTOR2 & b );

    //! Add point to container
    void push( const D3DXVECTOR2 & v );
    //! Insert point at given position
    void insert( unsigned int index, const D3DXVECTOR2 & v );
    //! Erase point at given position
    void erase( unsigned int index );
    //! Get item at given position
    D3DXVECTOR2 getitem( unsigned int index );
    //! Set item at given position
    void setitem( unsigned int index, const D3DXVECTOR2 & v );
    //! Get number of points
    inline unsigned int len() const { return points.size(); }
    //! Clear points
    inline void clear() { points.clear(); mAABox = AxisAlignedBox(); }

    //! Comparison operator
    inline bool __nonzero__() { return true; }

    //! Load points from file
    void load( const DirectoryId & path );

    // Points iterator
    typedef std::vector<D3DXVECTOR2>::const_iterator ConstPointsIterator;

    //! Function called when points changes
    virtual void on_points_change()
    {
        if (!points.size())
        {
            mAABox = AxisAlignedBox();
        }
        else
        {
            mAABox = AxisAlignedBox(points[0], points[0]);
            for (ConstPointsIterator i = points.begin(); i != points.end(); ++i)
                mAABox += *i;
        }
    }

    //! Axis aligned box
    const AxisAlignedBox & getAABox( void ) const { return mAABox; }

    //! Iterate through points
    ConstPointsIterator points_begin() const { return points.begin(); }
    ConstPointsIterator points_end() const { return points.end(); }

    //! Points
    std::vector<D3DXVECTOR2> points;
    
    // Points file signature
    static const DWORD signature = MAKELONG(MAKEWORD('R', 'G'), MAKEWORD('N', '0'));

    // Friend
    friend class GameObject;

private:
    AxisAlignedBox mAABox;

    friend void ::exportPointContainer( void );
};

}
