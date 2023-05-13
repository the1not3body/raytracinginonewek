// An abstatrction for hittable object
#ifndef HITTABLE_H
#define HITTABLE_H

// #include "ray.h"
#include "rtweekend.h"

class material;

struct hit_record {
    point3 p; // 碰撞点
    vec3 normal; // 碰撞点处的法向量
    shared_ptr<material> mat_ptr; // 材质
    double t; // 光线进行的长度
    bool front_face; // 如果小于0 说明光线来自于外部
    // 现在要判断光线碰撞点上，所碰撞的光线是来自于物体内部 还是物体外部。 比如说玻璃球
    // 处理的方法是 假设碰撞点的法向量总是超向外部
    // 如果光线方向和该点的法向量方向一致，那么我们认为这一点来自于光线的内部
    // 如果光线方向和该点法向量方向相反，那么则来自于外部
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0; // 如果小于0 说明光线来自于外部
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
    public: // 虚函数
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};


#endif