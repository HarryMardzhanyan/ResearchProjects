#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Image* load_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }
    
    // Чтение заголовков
    BMPHeader header;
    BMPInfoHeader info_header;
    
    if (fread(&header, sizeof(BMPHeader), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Error: Cannot read BMP header\n");
        return NULL;
    }
    
    // Проверка сигнатуры
    if (header.type != 0x4D42) { // "BM"
        fclose(file);
        fprintf(stderr, "Error: Not a BMP file\n");
        return NULL;
    }
    
    if (fread(&info_header, sizeof(BMPInfoHeader), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Error: Cannot read BMP info header\n");
        return NULL;
    }
    
    // Проверка формата (24-битный без сжатия)
    if (info_header.bits_per_pixel != 24 || info_header.compression != 0) {
        fclose(file);
        fprintf(stderr, "Error: Only 24-bit uncompressed BMP supported\n");
        return NULL;
    }
    
    // Высота может быть отрицательной (top-down)
    int height = abs(info_header.height);
    int width = info_header.width;
    
    // Выделение памяти для изображения
    Image* image = (Image*)malloc(sizeof(Image));
    if (!image) {
        fclose(file);
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    image->width = width;
    image->height = height;
    image->data = (Pixel*)malloc(width * height * sizeof(Pixel));
    
    if (!image->data) {
        fclose(file);
        free(image);
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    // Переход к данным пикселей
    fseek(file, header.offset, SEEK_SET);
    
    // Вычисление размера строки с выравниванием
    int row_padded = (width * 3 + 3) & (~3);
    uint8_t* row_buffer = (uint8_t*)malloc(row_padded);
    
    // Чтение данных построчно (BMP хранится снизу вверх)
    for (int y = height - 1; y >= 0; y--) {
        if (fread(row_buffer, 1, row_padded, file) != row_padded) {
            free(row_buffer);
            free_image(image);
            fclose(file);
            fprintf(stderr, "Error: Cannot read pixel data\n");
            return NULL;
        }
        
        for (int x = 0; x < width; x++) {
            image->data[y * width + x].blue = row_buffer[x * 3];
            image->data[y * width + x].green = row_buffer[x * 3 + 1];
            image->data[y * width + x].red = row_buffer[x * 3 + 2];
        }
    }
    
    free(row_buffer);
    fclose(file);
    return image;
}

int save_bmp(const char* filename, Image* image) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot create file %s\n", filename);
        return 0;
    }
    
    // Вычисление размера строки с выравниванием
    int width = image->width;
    int height = image->height;
    int row_padded = (width * 3 + 3) & (~3);
    int image_size = row_padded * height;
    
    // Заполнение заголовков
    BMPHeader header;
    BMPInfoHeader info_header;
    
    header.type = 0x4D42;
    header.size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + image_size;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);
    
    info_header.size = sizeof(BMPInfoHeader);
    info_header.width = width;
    info_header.height = height;  // Положительное - снизу вверх
    info_header.planes = 1;
    info_header.bits_per_pixel = 24;
    info_header.compression = 0;
    info_header.image_size = image_size;
    info_header.x_pixels_per_meter = 2835;  // ~72 DPI
    info_header.y_pixels_per_meter = 2835;
    info_header.colors_used = 0;
    info_header.colors_important = 0;
    
    // Запись заголовков
    fwrite(&header, sizeof(BMPHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);
    
    // Запись данных пикселей
    uint8_t* row_buffer = (uint8_t*)calloc(row_padded, 1);
    
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Pixel pixel = image->data[y * width + x];
            row_buffer[x * 3] = pixel.blue;
            row_buffer[x * 3 + 1] = pixel.green;
            row_buffer[x * 3 + 2] = pixel.red;
        }
        fwrite(row_buffer, 1, row_padded, file);
    }
    
    free(row_buffer);
    fclose(file);
    return 1;
}

void free_image(Image* image) {
    if (image) {
        if (image->data) {
            free(image->data);
        }
        free(image);
    }
}