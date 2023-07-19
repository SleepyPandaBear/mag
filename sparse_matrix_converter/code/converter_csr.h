/*********************************************
* File - converter_csr.cpp
* Author - Miha
* Created - 04 jul 2023
* Description - 
* *******************************************/
#if !defined(CONVERTER_CSR_H)
#define CONVERTER_CSR_H

#include "converter_read_matrix_market.h"

struct csr
{
    void *RowPointers;
    void *Columns;
    void *Elements;

    matrix_market_object Object;
    matrix_market_format Format;
    matrix_market_field Field;
    matrix_market_symmetry Symmetry;

    u32 FieldSize;

    u32 NumberOfRows;
    u32 NumberOfColumns;
    u32 NonZeroElements;
};

u32
CSR_ConvertFromMatrixMarket(csr *CSR, matrix_market *MatrixMarket)
{
    CSR->RowPointers = malloc((MatrixMarket->Header.NumberOfRows+1) * MatrixMarket->Header.FieldSize);
    if(CSR->RowPointers == NULL)
    {
        return MALLOC_ERR;
    }
    Memset(CSR->RowPointers, 0, (MatrixMarket->Header.NumberOfRows+1) * MatrixMarket->Header.FieldSize); 
    CSR->Columns = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(CSR->Columns == NULL)
    {
        return MALLOC_ERR;
    }
    CSR->Elements = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(CSR->Elements == NULL)
    {
        return MALLOC_ERR;
    }

    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        if(MatrixMarket->Header.FieldSize == 4)
        {
            MM_ToArray(CSR->RowPointers, u32)[MM_ToArray(MatrixMarket->Rows, u32)[I] + 1]++;
        }
        else if(MatrixMarket->Header.FieldSize == 8)
        {
            u64 Element = ((u64 *)MatrixMarket->Elements)[I];
        }
        else
        {
            return UNKNOWN_VALUE_ERR;
        }
    }
    for(u32 I = 1; I < MatrixMarket->Header.NumberOfRows+1; ++I)
    {
        MM_ToArray(CSR->RowPointers, u32)[I] += MM_ToArray(CSR->RowPointers, u32)[I-1];
    }

    u32 Length = MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize;
    CopyMemory(CSR->Columns, MatrixMarket->Columns, Length);
    CopyMemory(CSR->Elements, MatrixMarket->Elements, Length);

    CSR->FieldSize = MatrixMarket->Header.FieldSize;
    CSR->NumberOfRows = MatrixMarket->Header.NumberOfRows;
    CSR->NumberOfColumns = MatrixMarket->Header.NumberOfColumns;
    CSR->NonZeroElements = MatrixMarket->Header.NonZeroElements;

    CSR->Object = MatrixMarket->Header.Object;
    CSR->Format = MatrixMarket->Header.Format;
    CSR->Field = MatrixMarket->Header.Field;
    CSR->Symmetry = MatrixMarket->Header.Symmetry;

    return 0;
}

#endif // CONVERTER_CSR_H
