#ifndef BMP_H
#define BMP_H

#include "color.h"

#pragma pack(push, 1)
// Структура заголовка BMP файла
typedef struct {
    uint16_t type;              // Сигнатура "BM"
    uint32_t size;              // Размер файла
    uint16_t reserved1;         // Зарезервировано
    uint16_t reserved2;         // Зарезервировано
    uint32_t offset;            // Смещение до данных пикселей
} BMPHeader;

// Структура информации о BMP (BITMAPINFOHEADER)
typedef struct {
    uint32_t size;              // Размер этой структуры (40)
    int32_t width;              // Ширина изображения
    int32_t height;             // Высота изображения (положительное - снизу вверх)
    uint16_t planes;            // Количество плоскостей (1)
    uint16_t bits_per_pixel;    // Бит на пиксель (24)
    uint32_t compression;       // Тип сжатия (0 - без сжатия)
    uint32_t image_size;        // Размер данных изображения
    int32_t x_pixels_per_meter; // Горизонтальное разрешение
    int32_t y_pixels_per_meter; // Вертикальное разрешение
    uint32_t colors_used;       // Количество используемых цветов
    uint32_t colors_important;  // Количество важных цветов
} BMPInfoHeader;
#pragma pack(pop)

// Функции для работы с BMP
Image* load_bmp(const char* filename);
int save_bmp(const char* filename, Image* image);
void free_image(Image* image);

#endif