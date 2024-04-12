/*********************************************
* File - blas_ell.h
* Author - Miha
* Created - 06 jul 2023
* Description - 
* *******************************************/
#if !defined(BLAS_ELL_H)
#define BLAS_ELL_H

#include "converter_ell.h"
#include "blas.h"

u32
ELL_SparseMatrixVectorMultiplication(ell *ELL, vector *In, vector *Out)
{
    for(u32 I = 0; I < ELL->Header.NumberOfRows; ++I)
    {
        for(u32 J = 0; J < ELL->ElementsPerRow; ++J)
        {
            u32 ColumnIndex = ((u32 *)ELL->Columns)[I*ELL->ElementsPerRow + J];
            ((f32 *)Out->Elements)[I] += ((f32 *)ELL->Elements)[I*ELL->ElementsPerRow + J] * ((f32 *)In->Elements)[ColumnIndex];
        }
    }

    return 0;
}

u32
ELL_SpMV(ell *ELL, vector *In, vector *Out)
{
    u32 Error = ELL_SparseMatrixVectorMultiplication(ELL, In, Out);
    return Error;
}

#endif // BLAS_ELL_H
