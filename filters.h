#ifndef FILTERS_H
#define FILTERS_H

#include "color.h"

// 6 базовых фильтров
void crop_filter(Image* image, int new_width, int new_height);
void grayscale_filter(Image* image);
void negative_filter(Image* image);
void sharpening_filter(Image* image);
void edge_detection_filter(Image* image, float threshold);
void gaussian_blur_filter(Image* image, float sigma);

// Вспомогательные функции
Pixel get_pixel_clamped(Image* image, int x, int y);
void apply_matrix_filter(Image* image, float kernel[3][3]);

#endif