#include "filters.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Вспомогательная функция для безопасного доступа к пикселям
Pixel get_pixel_clamped(Image* image, int x, int y) {
    // Ограничение координат в пределах изображения
    x = (x < 0) ? 0 : ((x >= image->width) ? image->width - 1 : x);
    y = (y < 0) ? 0 : ((y >= image->height) ? image->height - 1 : y);
    return image->data[y * image->width + x];
}

// 1. Фильтр обрезки (Crop)
void crop_filter(Image* image, int new_width, int new_height) {
    // Ограничение размеров, если запрошенные больше исходных
    new_width = (new_width > image->width) ? image->width : new_width;
    new_height = (new_height > image->height) ? image->height : new_height;
    
    // Создание нового массива пикселей
    Pixel* new_data = (Pixel*)malloc(new_width * new_height * sizeof(Pixel));
    
    // Копирование данных из верхнего левого угла
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            new_data[y * new_width + x] = image->data[y * image->width + x];
        }
    }
    
    // Освобождение старого массива и обновление структуры
    free(image->data);
    image->data = new_data;
    image->width = new_width;
    image->height = new_height;
}

// 2. Фильтр оттенков серого (Grayscale)
void grayscale_filter(Image* image) {
    for (int i = 0; i < image->width * image->height; i++) {
        Pixel pixel = image->data[i];
        // Формула для преобразования в оттенки серого
        float gray = 0.299f * (pixel.red / 255.0f) +
                     0.587f * (pixel.green / 255.0f) +
                     0.114f * (pixel.blue / 255.0f);
        
        uint8_t gray_value = (uint8_t)(gray * 255);
        image->data[i].red = gray_value;
        image->data[i].green = gray_value;
        image->data[i].blue = gray_value;
    }
}

// 3. Фильтр негатива (Negative)
void negative_filter(Image* image) {
    for (int i = 0; i < image->width * image->height; i++) {
        image->data[i].red = 255 - image->data[i].red;
        image->data[i].green = 255 - image->data[i].green;
        image->data[i].blue = 255 - image->data[i].blue;
    }
}

// 4. Фильтр повышения резкости (Sharpening)
void sharpening_filter(Image* image) {
    // Матрица для повышения резкости
    float kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };
    
    apply_matrix_filter(image, kernel);
}

// 5. Фильтр обнаружения границ (Edge Detection)
void edge_detection_filter(Image* image, float threshold) {
    // Сначала преобразуем в оттенки серого
    grayscale_filter(image);
    
    // Матрица для обнаружения границ
    float kernel[3][3] = {
        {-1, -1, -1},
        {-1, 8, -1},
        {-1, -1, -1}
    };
    
    // Создаем копию изображения для применения фильтра
    Image* temp_image = (Image*)malloc(sizeof(Image));
    temp_image->width = image->width;
    temp_image->height = image->height;
    temp_image->data = (Pixel*)malloc(image->width * image->height * sizeof(Pixel));
    memcpy(temp_image->data, image->data, image->width * image->height * sizeof(Pixel));
    
    // Применяем матричный фильтр
    apply_matrix_filter(temp_image, kernel);
    
    // Применяем пороговое значение
    for (int i = 0; i < temp_image->width * temp_image->height; i++) {
        float gray = temp_image->data[i].red / 255.0f;
        if (gray > threshold) {
            image->data[i].red = 255;
            image->data[i].green = 255;
            image->data[i].blue = 255;
        } else {
            image->data[i].red = 0;
            image->data[i].green = 0;
            image->data[i].blue = 0;
        }
    }
    
    free(temp_image->data);
    free(temp_image);
}

// 6. Фильтр гауссова размытия (Gaussian Blur)
void gaussian_blur_filter(Image* image, float sigma) {
    if (sigma <= 0) return;
    
    int radius = (int)ceil(3 * sigma);  // Радиус ядра
    
    // Создание гауссова ядра
    int kernel_size = 2 * radius + 1;
    float* kernel = (float*)malloc(kernel_size * kernel_size * sizeof(float));
    
    float sum = 0.0f;
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            float value = expf(-(x*x + y*y) / (2 * sigma * sigma));
            kernel[(y + radius) * kernel_size + (x + radius)] = value;
            sum += value;
        }
    }
    
    // Нормализация ядра
    for (int i = 0; i < kernel_size * kernel_size; i++) {
        kernel[i] /= sum;
    }
    
    // Создание временного изображения
    Image* temp_image = (Image*)malloc(sizeof(Image));
    temp_image->width = image->width;
    temp_image->height = image->height;
    temp_image->data = (Pixel*)malloc(image->width * image->height * sizeof(Pixel));
    
    // Применение свертки
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            float r = 0, g = 0, b = 0;
            
            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    Pixel pixel = get_pixel_clamped(image, x + kx, y + ky);
                    float weight = kernel[(ky + radius) * kernel_size + (kx + radius)];
                    
                    r += pixel.red / 255.0f * weight;
                    g += pixel.green / 255.0f * weight;
                    b += pixel.blue / 255.0f * weight;
                }
            }
            
            // Ограничение значений
            r = fmaxf(0.0f, fminf(1.0f, r));
            g = fmaxf(0.0f, fminf(1.0f, g));
            b = fmaxf(0.0f, fminf(1.0f, b));
            
            temp_image->data[y * image->width + x].red = (uint8_t)(r * 255);
            temp_image->data[y * image->width + x].green = (uint8_t)(g * 255);
            temp_image->data[y * image->width + x].blue = (uint8_t)(b * 255);
        }
    }
    
    // Копирование результата обратно
    memcpy(image->data, temp_image->data, image->width * image->height * sizeof(Pixel));
    
    free(kernel);
    free(temp_image->data);
    free(temp_image);
}

// 7. Фильтр виньетки (Vignette)
void vignette_filter(Image* image, float strength) {
    // Параметры по умолчанию, если переданы некорректные значения
    if (strength <= 0) strength = 0.8f;
    float inner_radius = 0.01f;
    
    // Центр изображения
    float center_x = image->width / 2.0f;
    float center_y = image->height / 2.0f;
    
    // Максимальное расстояние от центра до угла
    float max_distance = sqrtf(center_x * center_x + center_y * center_y);
    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // Расстояние от текущего пикселя до центра
            float dx = x - center_x;
            float dy = y - center_y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            // Нормализованное расстояние [0, 1]
            float normalized_distance = distance / max_distance;
            
            // Коэффициент затемнения (1.0 - нет затемнения, 0.0 - полное затемнение)
            float darken_factor;
            
            if (normalized_distance <= inner_radius) {
                // Внутренняя область без затемнения
                darken_factor = 1.0f;
            } else {
                // Внешняя область с плавным затемнением
                float t = (normalized_distance - inner_radius) / (1.0f - inner_radius);
                // Используем квадратичную или квадратный корень для плавности
                darken_factor = 1.0f - strength * t * t - 0.1; // Квадратичное затемнение
                
                // Ограничиваем коэффициент
                if (darken_factor < 0.0f) darken_factor = 0.0f;
            }
            
            // Применяем затемнение к пикселю
            Pixel* pixel = &image->data[y * image->width + x];
            
            pixel->red = (uint8_t)(pixel->red * darken_factor);
            pixel->green = (uint8_t)(pixel->green * darken_factor);
            pixel->blue = (uint8_t)(pixel->blue * darken_factor);
        }
    }
}

// 8. Фильтр приближения и размытия (Zoom Blur)
void zoom_blur_filter(Image* image, float strength) {
    // Ограничиваем strength от 0 до 1
    if (strength < 0.0f) strength = 0.0f;
    if (strength > 1.0f) strength = 1.0f;
    
    // Находим центр изображения
    float center_x = image->width / 2.0f;
    float center_y = image->height / 2.0f;
    
    // Создаем временный буфер для результата
    Pixel* temp_data = (Pixel*)malloc(image->width * image->height * sizeof(Pixel));
    if (!temp_data) return;
    
    // Коэффициент для радиуса размытия (чем больше strength, тем больше радиус)
    float max_radius = 50.0f * strength;
    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            float dx = x - center_x;
            float dy = y - center_y;
            float distance = sqrtf(dx*dx + dy*dy);
            
            // Количество сэмплов зависит от расстояния от центра
            int samples = 5 + (int)(distance * 0.1f * strength);
            samples = (samples < 3) ? 3 : samples;
            
            float total_r = 0, total_g = 0, total_b = 0;
            
            for (int i = 0; i < samples; i++) {
                // Смещение зависит от distance и strength
                float factor = 1.0f + (max_radius * i / samples);
                factor *= (distance / (image->width/2)) * strength;
                
                // Вычисляем смещенные координаты к центру
                float offset_x = dx * (1.0f - factor);
                float offset_y = dy * (1.0f - factor);
                
                int sample_x = (int)(center_x + offset_x);
                int sample_y = (int)(center_y + offset_y);
                
                // Проверяем границы
                if (sample_x < 0) sample_x = 0;
                if (sample_x >= image->width) sample_x = image->width - 1;
                if (sample_y < 0) sample_y = 0;
                if (sample_y >= image->height) sample_y = image->height - 1;
                
                // Берем цвет из исходного изображения
                Pixel sample = image->data[sample_y * image->width + sample_x];
                total_r += sample.red;
                total_g += sample.green;
                total_b += sample.blue;
            }
            
            // Усредняем значения
            Pixel* result = &temp_data[y * image->width + x];
            result->red = (unsigned char)(total_r / samples);
            result->green = (unsigned char)(total_g / samples);
            result->blue = (unsigned char)(total_b / samples);
        }
    }
    
    // Копируем результат обратно
    memcpy(image->data, temp_data, image->width * image->height * sizeof(Pixel));
    free(temp_data);
}

// 9. Медианный фильтр (Median)
void median_filter(Image* image, int window_size) {
    if (window_size <= 0) window_size = 3;
    if (window_size % 2 == 0) window_size++;
    
    int radius = window_size / 2;
    int window_area = window_size * window_size;
    
    // Создаем гистограммы для каждого канала
    int hist_r[256] = {0};
    int hist_g[256] = {0};
    int hist_b[256] = {0};
    
    Image* temp_image = (Image*)malloc(sizeof(Image));
    temp_image->width = image->width;
    temp_image->height = image->height;
    temp_image->data = (Pixel*)malloc(image->width * image->height * sizeof(Pixel));
    
    for (int y = 0; y < image->height; y++) {
        // Сбрасываем гистограммы для каждой строки
        memset(hist_r, 0, sizeof(hist_r));
        memset(hist_g, 0, sizeof(hist_g));
        memset(hist_b, 0, sizeof(hist_b));
        
        // Инициализируем гистограммы для первого окна в строке
        for (int wy = -radius; wy <= radius; wy++) {
            for (int wx = -radius; wx <= radius; wx++) {
                Pixel p = get_pixel_clamped(image, wx, y + wy);
                hist_r[p.red]++;
                hist_g[p.green]++;
                hist_b[p.blue]++;
            }
        }
        
        for (int x = 0; x < image->width; x++) {
            // Находим медиану из гистограмм
            Pixel result;
            
            // Медиана для красного канала
            int count = 0;
            for (int i = 0; i < 256; i++) {
                count += hist_r[i];
                if (count > window_area / 2) {
                    result.red = i;
                    break;
                }
            }
            
            // Медиана для зеленого канала
            count = 0;
            for (int i = 0; i < 256; i++) {
                count += hist_g[i];
                if (count > window_area / 2) {
                    result.green = i;
                    break;
                }
            }
            
            // Медиана для синего канала
            count = 0;
            for (int i = 0; i < 256; i++) {
                count += hist_b[i];
                if (count > window_area / 2) {
                    result.blue = i;
                    break;
                }
            }
            
            temp_image->data[y * image->width + x] = result;
            
            // Обновляем гистограммы для следующего пикселя
            if (x + 1 < image->width) {
                // Удаляем левый столбец
                for (int wy = -radius; wy <= radius; wy++) {
                    Pixel p_old = get_pixel_clamped(image, x - radius, y + wy);
                    hist_r[p_old.red]--;
                    hist_g[p_old.green]--;
                    hist_b[p_old.blue]--;
                }
                
                // Добавляем правый столбец
                for (int wy = -radius; wy <= radius; wy++) {
                    Pixel p_new = get_pixel_clamped(image, x + radius + 1, y + wy);
                    hist_r[p_new.red]++;
                    hist_g[p_new.green]++;
                    hist_b[p_new.blue]++;
                }
            }
        }
    }

    
    memcpy(image->data, temp_image->data, image->width * image->height * sizeof(Pixel));
    free(temp_image->data);
    free(temp_image);
}

// Общая функция для применения матричного фильтра 3x3
void apply_matrix_filter(Image* image, float kernel[3][3]) {
    Image* temp_image = (Image*)malloc(sizeof(Image));
    temp_image->width = image->width;
    temp_image->height = image->height;
    temp_image->data = (Pixel*)malloc(image->width * image->height * sizeof(Pixel));
    
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            float r = 0, g = 0, b = 0;
            
            // Применение свертки 3x3
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    Pixel pixel = get_pixel_clamped(image, x + kx, y + ky);
                    float weight = kernel[ky + 1][kx + 1];
                    
                    r += pixel.red / 255.0f * weight;
                    g += pixel.green / 255.0f * weight;
                    b += pixel.blue / 255.0f * weight;
                }
            }
            
            // Ограничение значений
            r = fmaxf(0.0f, fminf(1.0f, r));
            g = fmaxf(0.0f, fminf(1.0f, g));
            b = fmaxf(0.0f, fminf(1.0f, b));
            
            temp_image->data[y * image->width + x].red = (uint8_t)(r * 255);
            temp_image->data[y * image->width + x].green = (uint8_t)(g * 255);
            temp_image->data[y * image->width + x].blue = (uint8_t)(b * 255);
        }
    }
    
    // Копирование результата обратно
    memcpy(image->data, temp_image->data, image->width * image->height * sizeof(Pixel));
    
    free(temp_image->data);
    free(temp_image);
}