#include <precompiled.h>
#include "Point_.h"

void exportPoint( void )
{
    using namespace bp;
    using namespace ingame;

    typedef Wrapper<Point> PointWrap;
    class_<PointWrap, bases<GameObject> >("Point")
        .def("update",              &Point::update, &PointWrap::default_update)
        .def("bezier",              &Point::bezier)
        ;
}

using namespace ingame;

float Point::distance( vec2 & point1, vec2 & point2 )
{
    return sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
}

vec2 Point::bezier( bp::list & points, float k )
{
    unsigned int len = bp::extract<unsigned int>(points.attr("__len__")());
    static std::vector<float> deltas;
    int l = static_cast<int>(len - 1);
    float c = 1.0, m = 1.0, n = 1.0;
    float x = 0.0, y = 0.0;

    deltas.reserve(len);
    deltas.resize(0);    

    for (unsigned int i = 0; i < len; ++i)
    {
        deltas.push_back(c * m);
        c = (l - i) * c / (i + 1);
        m *= k;
    }

    for (int i = l; i >= 0; --i)
    {
        m = deltas.back() * n; 
        deltas.pop_back();
        x += bp::extract<float>(points[i].attr("x")) * m;
        y += bp::extract<float>(points[i].attr("y")) * m;
        n *= (1 - k);
    }
    return vec2(x, y);
}

void Point::update( float /* dt */ )
{
    begin_update();
    end_update();
}
