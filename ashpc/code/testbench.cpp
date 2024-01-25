#include "stdio.h"

#include "kernel.h"

#include "converter_read_matrix_market.h"
#include "converter_block_coo.h"
#include "blas_coo.h"
#include "blas_bcoo.h"
#include "converter_coo.h"

int 
main() 
{
    FILE *File = fopen("dwt_512.mtx", "r");
    matrix_market MatrixMarket = {};
    MM_ReadFile(File, &MatrixMarket);
    MM_PrintHeader(&MatrixMarket.Header);
    MM_InsertSymetricValues(&MatrixMarket);

    bcoo BCOO = {};
    BCOO.BlockWidth = BLOCK_SIZE;
    BCOO.BlockHeight = BLOCK_SIZE;
    BCOO_ConvertFromMatrixMarket(&BCOO, &MatrixMarket);
    vector host_In = IdentityVector(BCOO.Header.NumberOfColumns, BCOO.Header.FieldSize);
    vector host_Out = ZeroVector(BCOO.Header.NumberOfRows, BCOO.Header.FieldSize);
    BCOO_SpMV(&BCOO, &host_In, &host_Out);

    // NOTE(miha): Convert to "simpler" format for the FPGA kernel. Need to have
    // predefined block size, because we can't have array of structs with each
    // struct with a pointer to the memory.
    block *KernelBlocks = (block *)malloc(BCOO.BlockLength * sizeof(block));
    int *In = (int *)malloc(BCOO.Header.NumberOfColumns * sizeof(int));
    int *Out = (int *)malloc(BCOO.Header.NumberOfRows * sizeof(int));

    for(int I = 0; I < BCOO.BlockLength; ++I)
    {
        KernelBlocks[I].Row = BCOO.Blocks[I].Row;
        KernelBlocks[I].Column = BCOO.Blocks[I].Column;
        for(int ElementIndex = 0; ElementIndex < BLOCK_SIZE * BLOCK_SIZE; ++ElementIndex)
        {
            KernelBlocks[I].Elements[ElementIndex] = MM_ToArray(BCOO.Blocks[I].Elements, u32)[ElementIndex];
        }

    }
    for(int I = 0; I < BCOO.Header.NumberOfColumns; ++I)
        In[I] = 1;
    for(int I = 0; I < BCOO.Header.NumberOfRows; ++I)
        Out[I] = 0;

    kernel(KernelBlocks, BCOO.BlockLength, In, Out);

    int Errors = 0;
    for(int I = 0; I < host_Out.Length; ++I)
    {
        if(MM_ToArray(host_Out.Elements, u32)[I] != Out[I])
            Errors++;
    }

    return Errors;
}
