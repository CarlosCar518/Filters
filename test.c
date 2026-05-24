#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

typedef struct
{
    uint8_t b, g, r;
} pixel24b_t;

typedef struct
{
    union
    {
        int rows, r;
    };
    union
    {
        int cols, c;
    };
    union
    {
        pixel24b_t *pixels, *p;
    };
    uint8_t type;
} sprite_t;

typedef void (*filter_fn)(pixel24b_t *);

uint8_t loadSprite(sprite_t *sprite, const char *filename)
{

    uint32_t offset;
    uint32_t rows;
    uint32_t cols;
    uint16_t bitsPerPixel;
    uint32_t pixelCount;
    uint32_t readPixels;

    printf("Loading image %s\n", filename);
    FILE *fileP;
    fileP = fopen(filename, "rb");
    if (fileP == NULL)
    {
        printf("Error loading image %s\n", filename);
        return 1;
    }
    if (fgetc(fileP) != 'B' || fgetc(fileP) != 'M')
    {
        printf("File is not an BMP\n");
        fclose(fileP);
        return 2;
    }
    fseek(fileP, 8, SEEK_CUR);
    fread(&offset, sizeof(uint32_t), 1, fileP);
    printf("Offset: %X", offset);
    fseek(fileP, 4, SEEK_CUR);
    fread(&cols, sizeof(uint32_t), 1, fileP);
    fread(&rows, sizeof(uint32_t), 1, fileP);
    fseek(fileP, 2, SEEK_CUR);
    fread(&bitsPerPixel, sizeof(uint16_t), 1, fileP);
    if (bitsPerPixel != 24)
    {
        printf("%d bits per pixel is not supported\n", bitsPerPixel);
        fclose(fileP);
        return 3;
    }

    sprite->r = rows;
    sprite->c = cols;
    sprite->type = bitsPerPixel / 8;
    pixelCount = (bitsPerPixel / 8) * rows * cols;
    sprite->pixels = malloc(pixelCount);
    if (sprite->pixels == NULL)
    {
        printf("Error allocating memory\n");
        fclose(fileP);
        return 4;
    }

    fseek(fileP, offset, SEEK_SET);
    readPixels = fread(sprite->pixels, pixelCount, 1, fileP);
    if (!readPixels)
    {
        printf("Error with the size in header and real size\n");
        free(sprite->pixels);
        return 5;
    }
    fclose(fileP);
    return 0;
}

uint8_t saveImage(sprite_t sprite, const char *ogFileName, const char *newFileName)
{
    FILE *fileOld;
    FILE *fileNew;
    uint32_t offset;
    if ((fileOld = fopen(ogFileName, "rb")) == NULL)
    {
        return 1;
    }
    if ((fileNew = fopen(newFileName, "wb")) == NULL)
    {
        fclose(fileOld);
        return 1;
    }
    fseek(fileOld, 10, SEEK_SET);
    fread(&offset, sizeof(uint32_t), 1, fileOld);
    fseek(fileOld, 0, SEEK_SET);

    uint8_t *headers = (uint8_t *)malloc(offset);
    if (headers == NULL)
    {
        fclose(fileOld);
        fclose(fileNew);
        return 2;
    }
    fread(headers, offset, 1, fileOld);
    fwrite(headers, offset, 1, fileNew);
    free(headers);
    fwrite(sprite.pixels, sizeof(pixel24b_t), sprite.c * sprite.r, fileNew);
    fclose(fileOld);
    fclose(fileNew);
    return 0;
}

void applyFilter(sprite_t *sprite, filter_fn filter)
{
    uint32_t rows = sprite->rows;
    uint32_t cols = sprite->cols;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            filter(&(sprite->pixels[i * cols + j]));
        }
    }
}

void grayFilter(pixel24b_t *pixel)
{
    uint8_t gray = (uint8_t)(0.299f * pixel->b + 0.587f * pixel->g + 0.114f * pixel->r);
    pixel->b = gray;
    pixel->r = gray;
    pixel->g = gray;
}

void negativeFilter(pixel24b_t *pixel)
{
    pixel->b = 255 - pixel->b;
    pixel->g = 255 - pixel->g;
    pixel->r = 255 - pixel->r;
}

uint8_t clamp(int value)
{
    if (value > 255)
        return 255;
    if (value < 0)
        return 0;
    return (uint8_t)value;
}

void vintageFilter(pixel24b_t *pixel)
{
    uint8_t original_b = pixel->b;
    uint8_t original_g = pixel->g;
    uint8_t original_r = pixel->r;
    pixel->b = clamp((int)(0.272f * original_r + 0.534f * original_g + 0.131f * original_b));
    pixel->g = clamp((int)(0.349f * original_r + 0.686f * original_g + 0.168f * original_b));
    pixel->r = clamp((int)(0.393f * original_r + 0.769f * original_g + 0.189f * original_b));
}

int main()
{
    sprite_t sprite;
    const char *fileName = "sample.bmp";
    if (loadSprite(&sprite, fileName))
    {
        return -1;
    }
    const char *nFileName = "test2.bmp";
    applyFilter(&sprite, vintageFilter);
    uint8_t retVal1;
    if ((retVal1 = saveImage(sprite, fileName, nFileName)) != 0)
        return retVal1;

    return 0;
}