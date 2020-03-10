
#ifndef __Geometry_hxx__
#define __Geometry_hxx__

#include <Point2D.hxx>
#include <Point3D.hxx>

// general purpose geometry functions

namespace Engine
{

/**
 * @brief Checks whether P1 and P2 are in the same side of the line defined by A and B.
 * 
 * @param P1 First Point2D.
 * @param P2 Second Point2D.
 * @param A First Point2D defining the line.
 * @param B Second Point2D defining the line.
 * @return true 
 * @return false 
 */
bool sameSide( Engine::Point2D<int> P1, Engine::Point2D<int> P2, Engine::Point2D<int> A, Engine::Point2D<int> B )
{
    Engine::Point3D<long long int> BA( B._x-A._x, B._y-A._y, 0 );
    Engine::Point3D<long long int> P1A( P1._x-A._x, P1._y-A._y, 0 );
    Engine::Point3D<long long int> P2A( P2._x-A._x, P2._y-A._y, 0 );

    Engine::Point3D<long long int> cp1 = BA.crossProduct( P1A );
    Engine::Point3D<long long int> cp2 = BA.crossProduct( P2A );

    if ( cp1.dot( cp2 )>=0 )
    {
        return true;
    }
    return false;
}

// 
/**
 * @brief Checks whether p is inside a triangle defined by the points ( 0, 0 ), b and c
 * 
 * @param p Point2D to check.
 * @param b Point2D defining the triangle.
 * @param c Point2D defining the triangle.
 * @return true 
 * @return false 
 */
bool insideTriangle( Engine::Point2D<int> p,  Engine::Point2D<int> b, Engine::Point2D<int> c )
{
    Engine::Point2D<int> a( 0, 0 );
    bool test1 =sameSide( p, a, b, c );
    bool test2 =sameSide( p, b, a, c );
    bool test3 =sameSide( p, c, a, b );

    if ( test1 && test2 && test3 )
    {
        return true;
    }
    return false;
}

} // namespace Engine

#endif // __Geometry_hxx__

