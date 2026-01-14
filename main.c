// gcc bmp.c filters.c main.c color.c -o main.exe
// ./main.exe Lena.bmp output.bmp -crop 600 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "filters.h"

// Функция для вывода справки
void print_help() {
    printf("  image_craft input.bmp output.bmp [-фильтр1 [параметры...]] [-фильтр2 [параметры...]] ...\n");
    printf("\nДоступные фильтры:\n");
    printf("  -crop width height    Обрезать изображение\n");
    printf("  -gs                   Преобразовать в оттенки серого\n");
    printf("  -neg                  Создать негатив\n");
    printf("  -sharp                Повысить резкость\n");
    printf("  -edge threshold       Обнаружить границы\n");
    printf("  -blur sigma           Применить гауссово размытие\n");
    printf("\nПример:\n");
    printf("  image_craft input.bmp output.bmp -crop 800 600 -gs -blur 0.5\n");
}

// Главная функция
int main(int argc, char* argv[]) {
    // Проверка минимального количества аргументов
    if (argc < 3) {
        print_help();
        return 0;
    }
    
    // Загрузка изображения
    Image* image = load_bmp(argv[1]);
    if (!image) {
        fprintf(stderr, "Failed to load image: %s\n", argv[1]);
        return 1;
    }
    
    printf("Image loaded: %dx%d pixels\n", image->width, image->height);
    
    // Обработка фильтров
    int i = 3;  // Начинаем с 3-го аргумента (первые два - пути к файлам)
    while (i < argc) {
        if (strcmp(argv[i], "-crop") == 0) {
            if (i + 2 < argc) {
                int width = atoi(argv[i + 1]);
                int height = atoi(argv[i + 2]);
                printf("Applying crop filter: %dx%d\n", width, height);
                crop_filter(image, width, height);
                i += 3;
            } else {
                fprintf(stderr, "Error: -crop requires width and height parameters\n");
                free_image(image);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-gs") == 0) {
            printf("Applying grayscale filter\n");
            grayscale_filter(image);
            i += 1;
        }
        else if (strcmp(argv[i], "-neg") == 0) {
            printf("Applying negative filter\n");
            negative_filter(image);
            i += 1;
        }
        else if (strcmp(argv[i], "-sharp") == 0) {
            printf("Applying sharpening filter\n");
            sharpening_filter(image);
            i += 1;
        }
        else if (strcmp(argv[i], "-edge") == 0) {
            if (i + 1 < argc) {
                float threshold = atof(argv[i + 1]);
                printf("Applying edge detection filter (threshold: %.2f)\n", threshold);
                edge_detection_filter(image, threshold);
                i += 2;
            } else {
                fprintf(stderr, "Error: -edge requires threshold parameter\n");
                free_image(image);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-blur") == 0) {
            if (i + 1 < argc) {
                float sigma = atof(argv[i + 1]);
                printf("Applying gaussian blur filter (sigma: %.2f)\n", sigma);
                gaussian_blur_filter(image, sigma);
                i += 2;
            } else {
                fprintf(stderr, "Error: -blur requires sigma parameter\n");
                free_image(image);
                return 1;
            }
        }
        else {
            fprintf(stderr, "Unknown filter: %s\n", argv[i]);
            free_image(image);
            return 1;
        }
    }
    
    // Сохранение результата
    if (save_bmp(argv[2], image)) {
        printf("Image saved to: %s\n", argv[2]);
    } else {
        fprintf(stderr, "Failed to save image: %s\n", argv[2]);
        free_image(image);
        return 1;
    }
    
    // Очистка памяти
    free_image(image);
    return 0;
}