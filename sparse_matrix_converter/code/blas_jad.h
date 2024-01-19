/*********************************************
* File - blas_jad.h
* Author - Miha
* Created - 01 aug 2023
* Description - 
* *******************************************/
#include "converter_read_matrix_market.h"
#include "converter_jad.h"
#include "blas.h"

#if !defined(BLAS_JAD_H)
#define BLAS_JAD_H

u32
JAD_SparseMatrixVectorMultiplication(jad *JAD, vector *In, vector *Out)
{
    if(JAD->Header.NumberOfColumns != In->Length)
        return 1; // TODO(miha): Use enum for error codes

    // TODO(miha): Should we also check if types are different?

    for(u32 I = 0; I < JAD->Header.NumberOfRows; ++I)
    {
        u32 RowStart = ((u32 *)JAD->RowPointers)[I];
        u32 RowEnd = ((u32 *)JAD->RowPointers)[I+1];
        for(u32 J = RowStart; J < RowEnd; ++J)
        {
            u32 ColumnIndex = ((u32 *)JAD->Columns)[J];
            MM_ToArray(Out->Elements, f32)[JAD->Permutations[J-RowStart]] += MM_ToArray(JAD->Elements, f32)[J] * MM_ToArray(In->Elements, f32)[ColumnIndex];
        }
    }

    return 0;
}

u32
JAD_SpMV(jad *JAD, vector *In, vector *Out)
{
    u32 Error = JAD_SparseMatrixVectorMultiplication(JAD, In, Out);
    return Error;
}

#endif // BLAS_JAD_H
