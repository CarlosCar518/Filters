#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdint.h>

typedef struct
{
    WORD signature;
    DWORD fileSize;
    DWORD unused;
    DWORD offset;
} BMPHeader;

typedef struct
{
    DWORD size;
    DWORD width;
    DWORD height;
    DWORD compression;
    DWORD compImgSize;
    DWORD xPixelsPerM;
    DWORD yPixelsPerM;
    DWORD colorsUsed;
    DWORD importantColors;
    WORD planes;
    WORD bpp;
} BMPInfoHeader;

char *getFileName()
{
    printf("Type file name: ");
    char *name = malloc(100 * sizeof(char));
    char c;
    int i = 0;
    while ((c = getchar()) != '\n' && c != EOF && i < 100)
        name[i++] = c;

    name[i] = '\0';
    return name;
}

FILE *readFile()
{
    FILE *fptr;                // ||======================================================================||
    char *name = "sample.bmp"; // ||HardCoded to avoid repetition. Change when finished.                  ||
    fptr = fopen(name, "rb");  // ||probably its best to take it as an argument when called on the shell. ||
    if (!fptr)                 // ||======================================================================||
        return NULL;
    return fptr;
}

void readHeader(BMPHeader *head, FILE *file)
{
    char c0 = fgetc(file);
    char c1 = fgetc(file);
    head->signature = (WORD)(c0 << 8 | c1);
    fread(&(head->fileSize), 3 * sizeof(DWORD), 1, file);
    return;
}
int main()
{
    BMPHeader *head = malloc(sizeof(BMPHeader));
    FILE *fptr = readFile();
    if (!fptr)
    {
        printf("Error reading file");
        return 1;
    }
    readHeader(head, fptr);
    printf("Firma en Hex: 0x%04X\nFile size: %i\nOffset: %i", head->signature, head->fileSize, head->offset);

    fclose(fptr);
    return 0;
}