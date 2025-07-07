#include "myLib.hpp"
#include "math.h"

size_t getFileSize(FILE* file)
{
    ASSERT(file, "file = NULL, it is impossible to work with null ptr", stderr);

    long currentPos = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    fseek(file, currentPos, SEEK_SET);

    return size;
}

char* readFileToBuffer(FILE* file)
{
    ASSERT(file, "file = NULL, it is impossible to work with null ptr", stderr);

    size_t size_file = getFileSize((file));

    char* buffer = (char*)(calloc(size_file + 1, sizeof(char)));
    ASSERT(buffer, "calloc return null buffer error", stderr);

    rewind(file);

    size_t read = fread(buffer, sizeof(char), size_file, file);
    if (read != (size_file))      
    {
        free(buffer);
        return nullptr;
    }

    buffer[size_file] = '\0';
    
    return buffer;
}

bool doubleCmp(double a, double b) 
{
    return (fabs(a - b) < DOUBLE_EPSILON);
}
