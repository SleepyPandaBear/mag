/*********************************************
 * File - blas_csr.h
 * Author - Miha
 * Created - 04 jul 2023
 * Description -
 * *******************************************/
#if !defined(BLAS_CSR_H)
#define BLAS_CSR_H

#include "blas.h"
#include "converter_csr.h"

u32
CSR_SparseMatrixVectorMultiplication(csr *CSR, vector *In, vector *Out)
{
    if(CSR->Header.NumberOfColumns != In->Length)
        return 1; // TODO(miha): Use enum for error codes

    // TODO(miha): Should we also check if types are different?

    for(u32 I = 0; I < CSR->Header.NumberOfRows; ++I)
    {
        u32 RowStart = ((u32 *)CSR->RowPointers)[I];
        u32 RowEnd   = ((u32 *)CSR->RowPointers)[I + 1];
        for(u32 J = RowStart; J < RowEnd; ++J)
        {
            u32 ColumnIndex = ((u32 *)CSR->Columns)[J];
            ((f32 *)Out->Elements)[I] += ((f32 *)CSR->Elements)[J] * ((f32 *)In->Elements)[ColumnIndex];
        }
    }

    return 0;
}

u32
CSR_SpMV(csr *CSR, vector *In, vector *Out)
{
    u32 Error = CSR_SparseMatrixVectorMultiplication(CSR, In, Out);
    return Error;
}

#endif // BLAS_CSR_H
