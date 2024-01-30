/*********************************************
* File - blas_coo.h
* Author - Miha
* Created - 01 jul 2023
* Description - 
* *******************************************/
#if !defined(BLAS_COO_H)
#define BLAS_COO_H

#include "converter_coo.h"
#include "blas.h"

u32
COO_SparseMatrixVectorMultiplication(coo *COO, vector *In, vector *Out)
{
    if(COO->Header.NumberOfColumns != In->Length)
        return 1; // TODO(miha): Use enum for error codes

    // TODO(miha): Should we also check if types are different?

    for(u32 I = 0; I < COO->Header.NonZeroElements; ++I)
    {
        u32 RowIndex = ((u32 *)COO->Rows)[I];
        u32 ColumnIndex = ((u32 *)COO->Columns)[I];
        ((u32 *)Out->Elements)[RowIndex] += ((u32 *)COO->Elements)[I] * ((u32 *)In->Elements)[ColumnIndex];
    }

    return 0;
}

u32
COO_SpMV(coo *COO, vector *In, vector *Out)
{
    u32 Error = COO_SparseMatrixVectorMultiplication(COO, In, Out);
    return Error;
}

#endif // BLAS_COO_H
