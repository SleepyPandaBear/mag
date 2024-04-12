/*********************************************
* File - blas_bcsr.h
* Author - Miha
* Created - 01 aug 2023
* Description - 
* *******************************************/
#if !defined(BLAS_BCSR_H)
#define BLAS_BCSR_H

#include "converter_bcsr.h"
#include "blas.h"

u32
BCSR_SparseMatrixVectorMultiplication(bcsr *BCSR, vector *In, vector *Out)
{
    if(BCSR->Header.NumberOfColumns != In->Length)
        return 1; // TODO(miha): Use enum for error codes

    // TODO(miha): Should we also check if types are different?

    for(u32 I = 0; I < BCSR->Header.NumberOfRows/BCSR->BlockSize; ++I)
    {
        u32 RowStart = ((u32 *)BCSR->RowPointers)[I];
        u32 RowEnd = ((u32 *)BCSR->RowPointers)[I+1];

        for(u32 BlockIndex = RowStart; BlockIndex < RowEnd; ++BlockIndex)
        {
            for(u32 ElementIndex = 0; ElementIndex < BCSR->BlockSize*BCSR->BlockSize; ++ElementIndex)
            {
                u32 GlobalRow = I*BCSR->BlockSize;
                u32 GlobalColumn = ((u32 *)BCSR->Columns)[BlockIndex]*BCSR->BlockSize;
                u32 Row = ElementIndex/BCSR->BlockSize;
                u32 Column = ElementIndex%BCSR->BlockSize;
                MM_ToArray(Out->Elements, f32)[GlobalRow+Row] += MM_ToArray(BCSR->Elements, f32)[BlockIndex*BCSR->BlockSize*BCSR->BlockSize+ElementIndex] * MM_ToArray(In->Elements, f32)[GlobalColumn+Column];
            }
        }
    }

    return 0;
}

u32
BCSR_SpMV(bcsr *BCSR, vector *In, vector *Out)
{
    u32 Error = BCSR_SparseMatrixVectorMultiplication(BCSR, In, Out);
    return Error;
}

#endif // BLAS_BCSR_H