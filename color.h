#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

// Структура для представления пикселя (24-битный BMP: BGR)
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

// Структура для представления изображения
typedef struct {
    Pixel* data;
    int width;
    int height;
} Image;

// Функции для работы с цветом
Pixel create_pixel(uint8_t r, uint8_t g, uint8_t b);
Pixel float_to_pixel(float r, float g, float b);

#endif