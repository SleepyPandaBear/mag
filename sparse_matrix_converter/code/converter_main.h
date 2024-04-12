/*********************************************
 * File - converter_main.h
 * Author - Miha
 * Created - 29 jun 2023
 * Description -
 * *******************************************/
#if !defined(CONVERTER_MAIN_H)
#define CONVERTER_MAIN_H

#include "stdint.h"
#include "stdio.h"

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

#define dtype i32

#define internal       static
#define local_persists static
#define global         static

#define Min(A, B)  (((A) < (B)) ? (A) : (B))
#define Max(A, B)  (((A) > (B)) ? (A) : (B))
#define CeilPos(X) ((X - (i32)(X)) > 0 ? (i32)(X + 1) : (i32)(X))
#define CeilNeg(X) ((X - (i32)(X)) < 0 ? (i32)(X - 1) : (i32)(X))
#define Ceil(X)    (((X) > 0) ? CeilPos(X) : CeilNeg(X))

// NOTE(miha): Compare two strings, string B is passed as const eg.
// CopareStrings(&S1, "constant string");
internal inline b32
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
internal inline b32
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
internal inline void
ToLower(char *String)
{
    while(*String != '\0')
    {
        *String = *String | (1 << 5);
        String++;
    }
}

internal inline void
CopyMemory(void *Destination, void *Source, u32 Length)
{
    u8 *D = (u8 *)Destination;
    u8 *S = (u8 *)Source;
    for(u32 I = 0; I < Length; ++I)
    {
        D[I] = S[I];
    }
}

internal inline void
Memset(void *Destination, u8 Value, u32 Length)
{
    u8 *D = (u8 *)Destination;
    for(u32 I = 0; I < Length; ++I)
    {
        D[I] = Value;
    }
}

internal inline i32
Abs(i32 Number)
{
    if(Number < 0)
        return -1 * Number;
    return Number;
}

#endif // CONVERTER_MAIN_H
