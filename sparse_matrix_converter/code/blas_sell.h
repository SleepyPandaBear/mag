/*********************************************
* File - blas_sell.h
* Author - Miha
* Created - 21 jul 2023
* Description - 
* *******************************************/
#if !defined(BLAS_SELL_H)
#define BLAS_SELL_H

#include "converter_sell.h"
#include "blas.h"

inline u32
SELL_SparseMatrixVectorMultiplication(sell *SELL, vector *In, vector *Out)
{
    for(u32 I = 0; I < SELL->Header.NumberOfRows; ++I)
    {
        u32 SliceIndex = I/SELL->SliceSize;
        u32 ElementsPerRow = (SELL->SliceOffset[SliceIndex+1]-SELL->SliceOffset[SliceIndex])/SELL->SliceSize;
        // TODO(miha): Do we really need this start index?
        u32 Start = SELL->SliceOffset[SliceIndex] + ((I%SELL->SliceSize)*ElementsPerRow);
        for(u32 J = 0; J < ElementsPerRow; ++J)
        {
            u32 ColumnIndex = ((u32 *)SELL->Columns)[Start + J];
            ((f32 *)Out->Elements)[I] += ((f32 *)SELL->Elements)[Start + J] * ((f32 *)In->Elements)[ColumnIndex];
        }
    }

    return 0;
}

inline u32
SELL_SpMV(sell *SELL, vector *In, vector *Out)
{
    u32 Error = SELL_SparseMatrixVectorMultiplication(SELL, In, Out);
    return Error;
}

#endif // BLAS_SELL_H