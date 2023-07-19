/*********************************************
* File - converter_main.h
* Author - Miha
* Created - 29 jun 2023
* Description - 
* *******************************************/
#if !defined(CONVERTER_MAIN_H)
#define CONVERTER_MAIN_H

#include "stdio.h"
#include "stdint.h"

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef u32 b32;

// NOTE(miha): Compare two strings, string B is passed as const eg.
// CopareStrings(&S1, "constant string");
b32
CompareString(char *A, const char *B)
{
    while(*A != '\0' && *B != '\0')
    {
        if(*A != *B)
            return 0;
        A++;
        B++;
    }
    return 1;
}

// NOTE(miha): Compare two strings.
b32
CompareString(char *A, char *B)
{
    while(*A != '\0' && *B != '\0')
    {
        if(*A != *B)
            return 0;
        A++;
        B++;
    }
    return 1;
}

// NOTE(miha): Turns char to lower case (sets 5th bit to 0).
void
ToLower(char *String)
{
    while(*String != '\0')
    {
        *String = *String | (1<<5);
        String++;
    }
}

void
CopyMemory(void *Destination, void *Source, u32 Length)
{
    u8 *D = (u8 *) Destination;
    u8 *S = (u8 *) Source;
    for(u32 I = 0; I < Length; ++I)
    {
        D[I] = S[I];
    }
}

void
Memset(void *Destination, u8 Value, u32 Length)
{
    u8 *D = (u8 *) Destination;
    for(u32 I = 0; I < Length; ++I)
    {
        D[I] = Value;
    }
}

#endif // CONVERTER_MAIN_H
