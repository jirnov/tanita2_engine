#pragma once
#include "micropather.h"
#include "Region_.h"

//Forward declaration
void exportPathFindRegion( void );

//! Namespace for game object type declarations
namespace ingame
{

//! Region with path finding capability
class PathFindRegion: public Region
{
public:
    //! Constructor
    PathFindRegion();
    //! Destructor
    virtual ~PathFindRegion() { if (pather) delete pather; }

    static void create_bindings();
    
protected:
    //! Get path from point A to point B (or empty path if none)
    Path find_path( const D3DXVECTOR2 & A, const D3DXVECTOR2 & B );
    
    //! Function called when points changes
    virtual void on_points_change();
    
    //! List of block regions
    bp::list block_regions;
    
private:
    // MicroPather Graph interface implementation
    class RegionMap: public micropather::Graph
    {
    public:
        // Constructor
        inline RegionMap() : width(0), height(0) {}
    
        // Return least cost estimation between two states
        virtual float LeastCostEstimate( void * start, void * end );
        // Fill adjacency array for given state
        virtual void AdjacentCost( void * state,
                                   std::vector<micropather::StateCost> * adjacent );
        // Print state in readable form
        virtual void PrintStateInfo( void * /* state */ ) {}
        
        // Nodes list type definition
        typedef std::vector<int> NodeList;
        typedef std::vector<int>::iterator NodeIter;
        
        // List of node coordinates
        NodeList nodes;
        // Map width and height
        int width, height;
        // Nodes offset
        int left, down;
        // Grid step size
        static const int grid_size = 20;
    };
    // Map object used in path finding
    RegionMap map;
    // MicroPather object
    micropather::MicroPather * pather;
    
    // Helper function for path straighting
    bool is_straight( int ax, int ay, int bx, int by );
    
    // Friend
    friend class GameObject;

private:
    friend void ::exportPathFindRegion( void );
};

}
