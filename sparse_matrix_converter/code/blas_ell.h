/*********************************************
* File - blas_ell.h
* Author - Miha
* Created - 06 jul 2023
* Description - 
* *******************************************/
#if !defined(BLAS_ELL_H)
#define BLAS_ELL_H

#include "blas.h"

u32
ELL_SparseMatrixVectorMultiplication(ell *ELL, vector *In, vector *Out)
{
    for(u32 I = 0; I < ELL->NumberOfRows; ++I)
    {
        for(u32 J = 0; J < ELL->NumberOfColumns; ++J)
        {
            u32 ColumnIndex = ((u32 *)ELL->Columns)[J*ELL->NumberOfRows + I];
            ((f32 *)Out->Elements)[I] += ((f32 *)ELL->Elements)[I*ELL->NumberOfColumns + J] * ((f32 *)In->Elements)[ColumnIndex];
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
