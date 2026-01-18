// gcc bmp.c filters.c main.c color.c -o main.exe
// ./main.exe Lena.bmp output.bmp -crop 100 100
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "filters.h"

// Функция для вывода справки
void print_help() {
    printf("\nREFERENCE\n");
    printf("Before running, compile the program using: gcc bmp.c filters.c main.c color.c -o main.exe\n");
    printf("  main.exe Lena.bmp output.bmp [-filter1 [parameters...]] [-filter2 [parameters...]] ...\n");
    printf("\nAvailable filters:\n");
    printf("  -crop width height    Crop image\n");
    printf("  -gs                   Convert to grayscale\n");
    printf("  -neg                  Create negativity\n");
    printf("  -sharp                Increase sharpness\n");
    printf("  -edge threshold       Highlighting the boundaries\n");
    printf("  -blur sigma           Apply Gaussian Blur\n");
    printf("  -vignette strength    Apply vignette (strength from 0 to 1)\n");
    printf("  -zoomblur strength    Apply zoom blur (strength from 0 to 1)\n");
    printf("  -med window           Apply median filter (window size is an odd number, minimum is 3)\n");
    printf("\nExample:\n");
    printf("  ./main.exe Lena.bmp output.bmp -crop 800 600 -gs -blur 0.5\n");
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
        else if (strcmp(argv[i], "-vignette") == 0) {
            if (i + 1 < argc) {
                float strength = atof(argv[i + 1]);
                if (strength < 0.0f) strength = 0.0f;
                if (strength > 1.0f) strength = 1.0f;
                
                printf("Applying vignette (strength: %.2f)\n", strength);
                vignette_filter(image, strength);
                i += 2;
            } else {
                printf("Error: -vignette requires strength parameter from 0 to 1\n");
                free_image(image);
                return 1;
            }
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
        else if (strcmp(argv[i], "-zoomblur") == 0) {
            if (i + 1 < argc) {
                float strength = atof(argv[i + 1]);
                if (strength < 0.0f) strength = 0.0f;
                if (strength > 1.0f) strength = 1.0f;
                printf("Applying zoom blur filter (strength: %.2f)\n", strength);
                zoom_blur_filter(image, strength);
                i += 2;
            } else {
                fprintf(stderr, "Error: -zoomblur requires strength parameter from 0 to 1\n");
                free_image(image);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-med") == 0) {
            if (i + 1 < argc) {
                float window = atof(argv[i + 1]);
                printf("Applying median filter (window: %.2f)\n", window);
                median_filter(image, window);
                i += 2;
            } else {
                fprintf(stderr, "Error: -med requires window parameter\n");
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