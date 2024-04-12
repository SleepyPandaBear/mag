/*********************************************
* File - blas_dia.h
* Author - Miha
* Created - 28 jul 2023
* Description - 
* *******************************************/
#if !defined(BLAS_DIA_H)
#define BLAS_DIA_H

#include "converter_dia.h"
#include "blas.h"

u32
DIA_SparseMatrixVectorMultiplication(dia *DIA, vector *In, vector *Out)
{
    if(DIA->Header.NumberOfColumns != In->Length)
        return 1; // TODO(miha): Use enum for error codes

    // TODO(miha): Should we also check if types are different?

    u32 MainDiagonalLength = Min(DIA->Header.NumberOfColumns, DIA->Header.NumberOfRows);
    for(u32 I = 0; I < DIA->NumberOfDiagonals; ++I)
    {
        for(u32 J = 0; J < MainDiagonalLength; ++J)
        {
            // NOTE(miha): Elements are above main diagonal.
            if(DIA->Offsets[I] > 0)
            {
                // NOTE(miha): In which column does the diagonal start.
                u32 ColumnStart = Abs(DIA->Offsets[I]);
                MM_ToArray(Out->Elements, f32)[J] += MM_ToArray(DIA->Elements, f32)[I*MainDiagonalLength + J] * MM_ToArray(In->Elements, f32)[ColumnStart+J];
            }
            else
            {
                // NOTE(miha): In which row does the diagonal start.
                u32 RowStart = Abs(DIA->Offsets[I]);
                MM_ToArray(Out->Elements, f32)[RowStart + J] += MM_ToArray(DIA->Elements, f32)[I*MainDiagonalLength+ J] * MM_ToArray(In->Elements, f32)[J];
            }
        }
    }

    return 0;
}

u32
DIA_SpMV(dia *DIA, vector *In, vector *Out)
{
    u32 Error = DIA_SparseMatrixVectorMultiplication(DIA, In, Out);
    return Error;
}


#endif // BLAS_DIA_H
