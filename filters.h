#ifndef FILTERS_H
#define FILTERS_H

#include "color.h"

// 9 фильтров
void crop_filter(Image* image, int new_width, int new_height);  // 1
void grayscale_filter(Image* image);  // 2
void negative_filter(Image* image);  // 3
void sharpening_filter(Image* image);  // 4
void edge_detection_filter(Image* image, float threshold);  // 5
void gaussian_blur_filter(Image* image, float sigma);  // 6
void vignette_filter(Image* image, float strength);  // 7
void zoom_blur_filter(Image* image, float strength);  // 8
void median_filter(Image* image, int window_size);  // 9

// Вспомогательные функции
Pixel get_pixel_clamped(Image* image, int x, int y);
void apply_matrix_filter(Image* image, float kernel[3][3]);

#endif