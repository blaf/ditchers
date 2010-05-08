#ifndef POINTHEADER
#define POINTHEADER

#define Point PointT<int>
#define Pointf PointT<double>

#define toInt to<int>
#define toDouble to<double>

#include "SDL_gfxPrimitives.h" //ceil, floor

template<class T>
class PointT {
    public:
    T x, y;

    void set(T newx, T newy){
        x = newx;
        y = newy;
    }

    T getX(){
        return x;
    }

    T getY(){
        return y;
    }

    PointT(){
        x = 0; y = 0;
    }

    PointT(T newx, T newy){
        x = newx; y = newy;
    }

    bool operator==(PointT pt){
        return (pt.x == x) && (pt.y == y);
    }

    bool operator!=(PointT pt){
        return (pt.x != x) || (pt.y != y);
    }

    PointT operator*(T coef){
        return PointT(x * coef, y * coef);
    }

    PointT operator/(T coef){
        return PointT(x / coef, y / coef);
    }

    PointT operator+(PointT pt){
        return PointT(x + pt.x, y + pt.y);
    }

    void operator+=(PointT pt){
        x += pt.x;
        y += pt.y;
    }

    PointT operator-(PointT pt){
        return PointT(x - pt.x, y - pt.y);
    }

    void operator-=(PointT pt){
        x -= pt.x;
        y -= pt.y;
    }

    PointT operator-(){
        return PointT(-x, -y);
    }

    PointT cw(){
        return PointT(y, -x);
    }

    PointT ccw(){
        return PointT(-y, x);
    }

    Point roundup(){
        return Point((int)ceil((double)x), (int)ceil((double)y));
    }

    Point rounddown(){
        return Point((int)floor((double)x), (int)floor((double)y));
    }

    void modulo(Point pt){
        while (x < 0) x += pt.x;
        while (y < 0) y += pt.y;
        while (x >= pt.x) x -= pt.x;
        while (y >= pt.y) y -= pt.y;
    }

    void bound(Point pt){
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > pt.x) x = pt.x;
        if (y > pt.y) y = pt.y;
    }

    bool inbox(Point pt){
        if ((x < 0) || (y < 0) || (x > pt.x) || (y > pt.y)) return false; else return true;
    }

    void outbox(Point pt){
        if ((x < 0) || (y < 0) || (x > pt.x) || (y > pt.y)) { x = -1; y = -1; }
    }

    template<class S>
    PointT<S> to(){
        return PointT<S>((S)x, (S)y);
    }
};

#endif