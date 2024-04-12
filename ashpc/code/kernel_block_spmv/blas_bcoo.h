/*********************************************
 * File - blas_bcoo.h
 * Author - Miha
 * Created - 20 jan 2024
 * Description -
 * *******************************************/
#if !defined(BLAS_BCOO_H)
#define BLAS_BCOO_H

#include "blas.h"
#include "converter_block_coo.h"

internal inline u32
BCOO_SparseMatrixVectorMultiplication(bcoo *BCOO, vector *In, vector *Out)
{
    if(BCOO->Header.NumberOfColumns != In->Length)
        return 1; // TODO(miha): Use enum for error codes

    // TODO(miha): Should we also check if types are different?

    // a1 a2   b1 b2   v1
    // a3 a4   b3 b4   v2
    //
    // c1 c2   d1 d2   v3
    // c3 c4   d3 d4   v4

    for(u32 BlockIndex = 0; BlockIndex < BCOO->BlockLength; ++BlockIndex)
    {
        bcoo_block Block = BCOO->Blocks[BlockIndex];
        u32 VectorStart  = Block.Column * BCOO->BlockHeight;
        for(u32 ElementIndex = 0; ElementIndex < BCOO->BlockWidth * BCOO->BlockHeight; ++ElementIndex)
        {
            u32 InIndex  = VectorStart + (ElementIndex % BCOO->BlockWidth);
            u32 OutIndex = (Block.Row * BCOO->BlockWidth) + (ElementIndex / BCOO->BlockWidth);

            MM_ToArray(Out->Elements, u32)[OutIndex] += MM_ToArray(In->Elements, u32)[InIndex] * MM_ToArray(Block.Elements, u32)[ElementIndex];
        }
    }

    return 0;
}

internal inline u32
BCOO_SpMV(bcoo *BCOO, vector *In, vector *Out)
{
    u32 Error = BCOO_SparseMatrixVectorMultiplication(BCOO, In, Out);
    return Error;
}

#endif // BLAS_BCOO_H
