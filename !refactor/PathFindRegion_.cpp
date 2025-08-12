#include <precompiled.h>
#include "PathFindRegion_.h"
#include "Path_.h"

void exportPathFindRegion( void )
{
    using namespace bp;
    using namespace ingame;

    typedef Wrapper<PathFindRegion> PathFindRegionWrap;
    class_<PathFindRegionWrap, bases<Region> >("PathFindRegion")
        .def("update",           &PathFindRegion::update,  &PathFindRegionWrap::default_update)
        .def("find_path",        &PathFindRegion::find_path)//, return_internal_reference<>())
        .def("is_point_inside",  &PathFindRegion::is_point_inside)
        
        .def_readwrite("block_regions", &PathFindRegion::block_regions)
        ;
}

using namespace ingame;
using namespace micropather;

// Return least cost estimation between two states
float PathFindRegion::RegionMap::LeastCostEstimate( void * start, void * end )
{
    int w = width * grid_size;
    int spos = int(start), sx = spos % w / grid_size, sy = spos / w / grid_size,
        epos = int(end),   ex = epos % w / grid_size, ey = epos / w / grid_size;
    return float(std::max(abs(ex - sx), abs(ey - sy)));
}

// Fill adjacency array for given state
void PathFindRegion::RegionMap::AdjacentCost( void * state, 
                                              std::vector<micropather::StateCost> * adjacent )
{
    int w = width * grid_size;
    int pos = int(state), xx = pos % w / grid_size, yy = pos / w / grid_size;
    
#define NEIGHBOURS 1
    float costs[NEIGHBOURS*2+1][NEIGHBOURS*2+1] = {{1.4f, 2, 1.4f}, {1, 0, 1}, {1.4f, 2, 1.4f}};

    for (int j = -NEIGHBOURS; j < NEIGHBOURS+1; ++j)
        for (int i = -NEIGHBOURS; i < NEIGHBOURS+1; ++i)
        {
            int x = xx+i, y = yy+j;
            
            if ((i == 0 && j == 0) || x < 0 || y < 0 || x >= width || y >= height ||
                3 != nodes[y * width + x])
                continue;

  #if NEIGHBOURS > 1
            if (i < -1 || i > 1 || j < -1 || j > 1)
            {
                int di = i < -1 ? 1 : i > 1 ? -1 : 0,
                    dj = j < -1 ? 1 : j > 1 ? -1 : 0;
                if (3 != nodes[(y+dj) * width + x + di]) continue;
            }
  #endif
            
            StateCost s;
            s.state = (void *)((y * w + x) * grid_size);
            s.cost = costs[NEIGHBOURS+j][NEIGHBOURS+i];
            adjacent->push_back(s);
        }
}

// Constructor
PathFindRegion::PathFindRegion() : pather(NULL)
{
    pather = new micropather::MicroPather(&map, 1024);
}

// Get path from point A to point B (or empty path if none)
Path PathFindRegion::find_path( const D3DXVECTOR2 & A, const D3DXVECTOR2 & B )
{
#define TOINT(a) (int(a) / RegionMap::grid_size * RegionMap::grid_size)
#define TOSTATE(x, y) (void *)(TOINT(y) * map.width * RegionMap::grid_size + TOINT(x))

    // Transforming points to local coordinates
    D3DXVECTOR4 va_tmp, vb_tmp;
    D3DXVec2Transform(&va_tmp, &A, &transformation_inv);
    D3DXVec2Transform(&vb_tmp, &B, &transformation_inv);
    D3DXVECTOR2 va(va_tmp.x - float(map.left), va_tmp.y - float(map.down)),
                vb(vb_tmp.x - float(map.left), vb_tmp.y - float(map.down));

    // Checking if destination point is blocked
    bool need_find_destination = false;
    if (!is_local_point_inside(D3DXVECTOR2(vb_tmp.x, vb_tmp.y)))
        need_find_destination = true;
    int block_reg_count = bp::extract<int>(block_regions.attr("__len__")());
    for (int i = 0; i < block_reg_count; ++i)
    {
        Region & r = bp::extract<Region &>(block_regions[i]);
        if (r.is_point_inside(B))
        {
            need_find_destination = true;
            break;
        }
    }
    
    // Updating node data (marking nodes blocked by regions with 1,
    // non-blocked with 3, nodes outside region by 0)
    for (int y = 0; y < map.height; ++y)
        for (int x = 0; x < map.width; ++x)
        {
            int & node = map.nodes[map.width * y + x];
            if (!node) continue;
            
            D3DXVECTOR2 p(float(map.left + x * RegionMap::grid_size), 
                          float(map.down + y * RegionMap::grid_size));
            p += get_absolute_position();
            
            node = 3;
            if (0 != block_reg_count)
                // Testing block regions
                for (int i = 0; i < block_reg_count; ++i)
                {
                    Region & r = bp::extract<Region &>(block_regions[i]);
                    if (r.is_point_inside(p))
                        node = 1;
                }
        }
    
    int ax = int(va.x) / RegionMap::grid_size,
        ay = int(va.y) / RegionMap::grid_size,
        bx = int(vb.x) / RegionMap::grid_size,
        by = int(vb.y) / RegionMap::grid_size;
    
    // Finding destination point inside region
    if (need_find_destination)
    {
        bool found = false;
        
        // Vertical line
        if (0 <= bx && bx < map.width)
        {
            int Dy = by - ay, dy = -1;
            if (Dy < 0) Dy = -Dy, dy = -dy;
            Dy++;
            
            const int by_clamp = by < 0 ? 0 : by,
                      vlimit = dy > 0 ? map.height - by_clamp : by_clamp;
            for (int j = 0; j < vlimit; ++j)
            {
                const int new_by = by_clamp + j * dy;
                if (new_by < 0 || new_by >= map.height)
                    continue;
                
                if (map.nodes[map.width * new_by + bx] == 3)
                {
                    const float vby = (float)new_by * RegionMap::grid_size,
                                vb_tmpy = vby + float(map.down);
                    D3DXVECTOR2 point(vb_tmp.x, vb_tmpy);
                    if (is_local_point_inside(point))
                    {
                        D3DXVECTOR4 pt4;
                        D3DXVec2Transform(&pt4, &point, &transformation);
                        D3DXVECTOR2 point_transformed(pt4.x, pt4.y);

                        bool blocked = false;
                        for (int i = 0; i < block_reg_count; ++i)
                        {
                            Region & r = bp::extract<Region &>(block_regions[i]);
                            if (r.is_point_inside(point_transformed))
                            {
                                blocked = true;
                                break;
                            }
                        }
                        if (!blocked)
                        {
                            vb.y = vby;
                            vb_tmp.y = vb_tmpy;
                            by = int(vby) / RegionMap::grid_size;
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
        // Horizontal line
        if (!found && 0 <= by && by < map.height)
        {
            if (by < map.height - 1)
                by++;
            int Dx = bx - ax, dx = -1;
            if (Dx < 0) Dx = -Dx, dx = -dx;
            
            const int bx_clamp = bx < 0 ? 0 : bx,
                      hlimit = dx > 0 ? map.width - bx_clamp : bx_clamp;
            for (int j = 0; j < hlimit; ++j)
            {
                const int new_bx = bx_clamp + j * dx;
                if (new_bx < 0 || new_bx >= map.width)
                    continue;
                
                if (map.nodes[map.width * by + new_bx] == 3)
                {
                    const float vbx = (float)new_bx * RegionMap::grid_size,
                                vb_tmpx = vbx + float(map.left);
                    D3DXVECTOR2 point(vb_tmpx, vb_tmp.y);
                    if (is_local_point_inside(point))
                    {
                        D3DXVECTOR4 pt4;
                        D3DXVec2Transform(&pt4, &point, &transformation);
                        D3DXVECTOR2 point_transformed(pt4.x, pt4.y);

                        bool blocked = false;
                        for (int i = 0; i < block_reg_count; ++i)
                        {
                            Region & r = bp::extract<Region &>(block_regions[i]);
                            if (r.is_point_inside(point_transformed))
                            {
                                blocked = true;
                                break;
                            }
                        }
                        if (!blocked)
                        {
                            int dbx = ax - int(vbx) / RegionMap::grid_size;
                            if ((ax - bx) * dbx >= 0)
                            {
                                vb.x = vbx;
                                vb_tmp.x = vb_tmpx;
                                bx = int(vbx) / RegionMap::grid_size;
                                found = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        // Horizontal line (at A level)
        if (!found && 0 <= ay && ay < map.height)
        {
            if (by < map.height - 1)
                by++;
            int Dx = bx - ax, dx = -1;
            if (Dx < 0) Dx = -Dx, dx = -dx;
            
            const int bx_clamp = bx < 0 ? 0 : bx,
                      hlimit = dx > 0 ? map.width - bx_clamp : bx_clamp;
            for (int j = 0; j < hlimit; ++j)
            {
                const int new_bx = bx_clamp + j * dx;
                if (new_bx < 0 || new_bx >= map.width)
                    continue;
                
                if (map.nodes[map.width * ay + new_bx] == 3)
                {
                    const float vbx = (float)new_bx * RegionMap::grid_size,
                                vb_tmpx = vbx + float(map.left);
                    if (is_local_point_inside(D3DXVECTOR2(vb_tmpx, va_tmp.y)))
                    {
                        //int dbx = ax - int(vbx) / RegionMap::grid_size;
                        //if ((ax - bx) * dbx >= 0)
                        {
                            vb.x = vbx;
                            vb.y = va.y;
                            vb_tmp.x = vb_tmpx;
                            vb_tmp.y = va_tmp.y;
                            bx = int(vbx) / RegionMap::grid_size;
                            by = ay;
                            found = true;
                            break;
                        }
                    }
                }
            }
        }        
        if (!found)
            return Path();
    }
    
    // Finding nearest non-blocked grid points for A and B
    int nearest_ax = ax, 
        nearest_bx = bx,
        
        nearest_ay = ay,
        nearest_by = by,
        
        best_dist_a = 1000,
        best_dist_b = 1000;
    
    int dy = (va.y > vb.y) ?  1 : -1,
        y0 = (dy > 0)      ? -1 :  1,
        y1 = (dy > 0)      ?  2 : -2,
        
        dx = (va.x > vb.x) ?  1 : -1,
        x0 = (dx > 0)      ? -1 :  1,
        x1 = (dx > 0)      ?  2 : -2;

    for (int j = y0; j != y1; j += dy)
        for (int i = x0; i != x1; i += dx)
        {
            int xa = ax + i, ya = ay + j;
            if (xa >= 0 && ya >= 0 && xa < map.width && ya < map.height &&
                map.nodes[map.width * ya + xa] == 3 && abs(i) + abs(j) < best_dist_a)
            {
                best_dist_a = abs(i) + abs(j);
                nearest_ax = xa;
                nearest_ay = ya;
            }
            
            int xb = bx - i, yb = by - j;
            if (xb >= 0 && yb >= 0 && xb < map.width && yb < map.height &&
                map.nodes[map.width * yb + xb] == 3 && abs(i) + abs(j) < best_dist_b)
            {
                best_dist_b = abs(i) + abs(j);
                nearest_bx = xb;
                nearest_by = yb;
            }
        }
    ax = nearest_ax * RegionMap::grid_size;
    ay = nearest_ay * RegionMap::grid_size;
    
    bx = nearest_bx * RegionMap::grid_size;
    by = nearest_by * RegionMap::grid_size;
 
    // Trying to solve path finding problem
    pather->Reset();
    std::vector<void *> path;
    float cost;
    int result = pather->Solve(TOSTATE(ax, ay), TOSTATE(bx, by), &path, &cost);
    if (MicroPather::NO_SOLUTION == result)
    {
        // Checking if source point is blocked
        if (is_local_point_inside(D3DXVECTOR2(va_tmp.x, va_tmp.y)))
        {
            int block_reg_count = bp::extract<int>(block_regions.attr("__len__")());
            bool is_blocked = false;
            for (int i = 0; i < block_reg_count; ++i)
            {
                Region & r = bp::extract<Region &>(block_regions[i]);
                if (r.is_point_inside(A))
                {
                    is_blocked = true;
                    break;
                }
            }
            if (!is_blocked)
                return Path();
        }
        
        // Ќаходим точку в floor, ближайшую к исходной и идем в нее (надо выйти из блок-региона).
        nearest_ax = 0;
        nearest_bx = 0;
        best_dist_a = 1000000;
        
        int srcX = int(va.x) / RegionMap::grid_size,
            srcY = int(va.y) / RegionMap::grid_size;
        
        for (int j = 0; j < map.height; ++j)
            for (int i = 0; i < map.width; ++i)
            {
                int di = i - srcX, dj = j - srcY;
                int dist2 = di * di + dj * dj;
                if (map.nodes[map.width * j + i] == 3 && dist2 < best_dist_a)
                {
                    best_dist_a = dist2;
                    nearest_ax = i;
                    nearest_ay = j;
                }
            }
        
        Path p;
        p.push(D3DXVECTOR2(va_tmp.x, va_tmp.y) + position);
        p.push(D3DXVECTOR2(nearest_ax * RegionMap::grid_size + float(map.left), nearest_ay * RegionMap::grid_size + float(map.down)) + position);
        return p;
    }
    
#undef TOSTATE
#undef TOINT
    
    // Returning empty path if ends are near
    if (MicroPather::START_END_SAME == result)
    {
        Path p;
        p.push(D3DXVECTOR2(va_tmp.x, va_tmp.y) + position);
        p.push(D3DXVECTOR2(vb_tmp.x, vb_tmp.y) + position);
        return p;
    }
    
    // Returning optimized path
    Path p;
    p.push(D3DXVECTOR2(va_tmp.x, va_tmp.y) + position);
    
    // Straightening path
    int sax = int(va_tmp.x), say = int(va_tmp.y);
    UINT k = 0;
    
    if (is_straight(sax, say, int(vb_tmp.x), int(vb_tmp.y)))
    {
        p.push(D3DXVECTOR2(vb_tmp.x, vb_tmp.y) + position);
        return p;
    }
    
    while (k < path.size()-1)
    {
        int w = map.width * RegionMap::grid_size,
            pos = int(path[k]),
            posn = int(path[k+1]);
        ++k;
        if (is_straight(sax, say, posn % w + map.left, posn / w + map.down))
            continue;
        
        p.push(D3DXVECTOR2(float(pos % w) + float(map.left), float(pos / w) + float(map.down)) + position);
        sax = pos % w + map.left;
        say = pos / w + map.down;
    }
    p.push(D3DXVECTOR2(vb_tmp.x, vb_tmp.y) + position);
    return p;
}

// Helper function for path straighting
bool PathFindRegion::is_straight( int ax, int ay, int bx, int by )
{
    int N = abs(ax - bx);
    if (abs(ay - by) > N)
        N = abs(ay - by);
    N /= RegionMap::grid_size / 2;
    if (0 == N) return true;
    int dx = (bx - ax) / N,
        dy = (by - ay) / N;
    const D3DXVECTOR2 abs_pos = get_absolute_position();

    int block_reg_count = bp::extract<int>(block_regions.attr("__len__")());
    for (int i = 1; i < N-1; ++i)
    {
        float x = float(ax + dx * i),
              y = float(ay + dy * i);
        D3DXVECTOR2 v = D3DXVECTOR2(x, y) + abs_pos;
    
        if (!is_local_point_inside(D3DXVECTOR2(x, y)))
            return false;
        for (int j = 0; j < block_reg_count; ++j)
        {
            Region & r = bp::extract<Region &>(block_regions[j]);
            if (r.is_point_inside(v))
                return false;
        }
    }
    return true;
}

// Function called when points changes
void PathFindRegion::on_points_change()
{
    const int inf = 0x7fffffff;
    map.left = map.down = inf;
    int right = -inf, up = -inf;
    for (ConstPointsIterator i = points.begin(); points.end() != i; ++i)
    {
        int x = int(i->x), y = int(i->y);
        
        if (x < map.left) map.left = x;
        if (x > right) right = x;
        if (y < map.down) map.down = y;
        if (y > up) up = y;
    }
    
    map.width = (right - map.left) / RegionMap::grid_size + 1;
    map.height = (up - map.down) / RegionMap::grid_size + 1;
    map.nodes.resize(map.width * map.height);
    for (int y = 0; y < map.height; ++y)
        for (int x = 0; x < map.width; ++x)
            map.nodes[map.width * y + x] = is_local_point_inside(D3DXVECTOR2(
                                               float(map.left + x * RegionMap::grid_size), 
                                               float(map.down + y * RegionMap::grid_size))) ? 1 : 0;
}
