#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    public:
        ray() {}
        ray(const point3& origin, const vec3& direction) 
            : orig(origin), dir(direction)
        {}

        point3 origin() const { return orig; }
        vec3 direction() const { return dir; }

        point3 at(double t) const { // 光线的表示方法
            return orig + t*dir;
        }

    public:
        point3 orig; // 光源起点
        vec3 dir; // 光源方向
};

#endif