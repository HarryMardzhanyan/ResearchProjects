#include "color.h"
#include <math.h>

Pixel create_pixel(uint8_t r, uint8_t g, uint8_t b) {
    Pixel pixel;
    pixel.red = r;
    pixel.green = g;
    pixel.blue = b;
    return pixel;
}

// Конвертация пикселя в значения с плавающей точкой [0, 1]
float pixel_to_float_component(uint8_t component) {
    return component / 255.0f;
}

Pixel float_to_pixel(float r, float g, float b) {
    // Ограничение значений в диапазоне [0, 1]
    r = fmaxf(0.0f, fminf(1.0f, r));
    g = fmaxf(0.0f, fminf(1.0f, g));
    b = fmaxf(0.0f, fminf(1.0f, b));
    
    Pixel pixel;
    pixel.red = (uint8_t)(r * 255.0f);
    pixel.green = (uint8_t)(g * 255.0f);
    pixel.blue = (uint8_t)(b * 255.0f);
    return pixel;
}