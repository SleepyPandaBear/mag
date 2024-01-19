/*********************************************
* File - converter_dia.h
* Author - Miha
* Created - 27 jul 2023
* Description - 
* *******************************************/
#include "converter_main.h"
#include "cstdint"
#include "converter_read_matrix_market.h"

#if !defined(CONVERTER_DIA_H)
#define CONVERTER_DIA_H

struct dia
{
    void *Elements;
    i32 *Offsets;

    u32 NumberOfDiagonals;
    u32 MainDiagonalLength;

    matrix_market_header Header;
};

u32
DIA_ConvertFromMatrixMarket(dia *DIA, matrix_market *MatrixMarket)
{
    // NOTE(miha): Formula for number of diagonals in the matrix is: #rows+#columns-1.
    u32 NumberOfDiagonals = MatrixMarket->Header.NumberOfRows+MatrixMarket->Header.NumberOfColumns-1;

    i32 Offsets[NumberOfDiagonals];

    // NOTE(miha): Fill Offsets with "inf", this way we can see which diagonals are used.
    for(u32 I = 0; I < NumberOfDiagonals; ++I)
        Offsets[I] = INT32_MAX;

    // NOTE(miha): First pass.
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        u32 R = MatrixMarket->Rows[I];
        u32 C = MatrixMarket->Columns[I];
        u32 MainDiagonalOffset = MatrixMarket->Header.NumberOfRows-1;
        Offsets[C-R+MainDiagonalOffset] = C-R;
    }

    // NOTE(miha): How many diagonals we actually have to save?
    u32 CountDiagonals = 0;
    for(u32 I = 0; I < NumberOfDiagonals; ++I)
    {
        if(Offsets[I] != INT32_MAX)
            CountDiagonals++;
    }

    DIA->Offsets = (i32 *)malloc(CountDiagonals*sizeof(i32));
    if(DIA->Offsets == NULL)
    {
        return MALLOC_ERR;
    }

    // NOTE(miha): Count how many "diagonals" we skip.
    u32 Pad = 0;

    // NOTE(miha): Fill Offset array without "inf" (unused) diagonals.
    for(u32 I = 0; I < NumberOfDiagonals; ++I)
    {
        if(Offsets[I] == INT32_MAX)
        {
            Pad++;
            continue;
        }

        DIA->Offsets[I-Pad] = Offsets[I];
    }

    u32 MainDiagonalLength = Min(MatrixMarket->Header.NumberOfColumns, MatrixMarket->Header.NumberOfRows);
    DIA->Elements = malloc(CountDiagonals*MainDiagonalLength*sizeof(MatrixMarket->Header.FieldSize));
    if(DIA->Elements == NULL)
    {
        return MALLOC_ERR;
    }

    // NOTE(miha): Second pass.
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        i32 R = (i32)MatrixMarket->Rows[I];
        i32 C = (i32)MatrixMarket->Columns[I];

        // NOTE(miha): Find where in array DIA->Elements we put element from MatrixMarket.
        u32 OffsetIndex = 0;
        while(C-R != DIA->Offsets[OffsetIndex])
            OffsetIndex++;

        // NOTE(miha): If C-R < 0 -> element is under diagonal, else elements is on diagonal or above.
        if((C-R) < 0)
            MM_ToArray(DIA->Elements, f32)[OffsetIndex*MainDiagonalLength + C] = MM_ToArray(MatrixMarket->Elements, f32)[I];
        else
            MM_ToArray(DIA->Elements, f32)[OffsetIndex*MainDiagonalLength + R] = MM_ToArray(MatrixMarket->Elements, f32)[I];
    }

    // TODO(miha): If this copies struct use it everywhere!
    DIA->Header = MatrixMarket->Header;
    DIA->NumberOfDiagonals = CountDiagonals;
    DIA->MainDiagonalLength = MainDiagonalLength;

    return 0;
}

#endif // CONVERTER_DIA_H
