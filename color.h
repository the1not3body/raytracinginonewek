#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include<iostream>

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // 实现漫反射后，产生的照片非常暗，但是可以看到球下面产生的阴影，
    // gamma矫正？？wtf is gamma correction dude u telling shit
    // 因为许多图像查看器都假设图像已经进行了处理 "gamma correction"
    // 
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r); // so-called gamma rate = 2.0
    g = sqrt(scale * g); // 
    b = sqrt(scale * b); 
    
    // write
    out << static_cast<int>(255.99 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(255.99 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(255.99 * clamp(b, 0.0, 0.999)) << '\n';
}


#endif