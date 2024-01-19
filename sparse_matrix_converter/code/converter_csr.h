/*********************************************
* File - converter_csr.cpp
* Author - Miha
* Created - 04 jul 2023
* Description - 
* *******************************************/
#include "converter_read_matrix_market.h"

#if !defined(CONVERTER_CSR_H)
#define CONVERTER_CSR_H

struct csr
{
    void *RowPointers;
    void *Columns;
    void *Elements;

    matrix_market_header Header;
};

internal inline u32
CSR_ConvertFromMatrixMarket(csr *CSR, matrix_market *MatrixMarket)
{
    CSR->Header = MatrixMarket->Header;
    matrix_market_header *Header = &CSR->Header;

    CSR->RowPointers = malloc((Header->NumberOfRows+1) * Header->FieldSize);
    if(CSR->RowPointers == NULL)
        return MALLOC_ERR;
    Memset(CSR->RowPointers, 0, (Header->NumberOfRows+1) * Header->FieldSize); 

    CSR->Columns = malloc(Header->NonZeroElements * Header->FieldSize);
    if(CSR->Columns == NULL)
        return MALLOC_ERR;

    CSR->Elements = malloc(Header->NonZeroElements * Header->FieldSize);
    if(CSR->Elements == NULL)
        return MALLOC_ERR;

    for(u32 I = 0; I < Header->NonZeroElements; ++I)
    {
        if(Header->FieldSize == 4)
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

    for(u32 I = 1; I < Header->NumberOfRows+1; ++I)
        MM_ToArray(CSR->RowPointers, u32)[I] += MM_ToArray(CSR->RowPointers, u32)[I-1];

    u32 Length = Header->NonZeroElements * Header->FieldSize;
    CopyMemory(CSR->Columns, MatrixMarket->Columns, Length);
    CopyMemory(CSR->Elements, MatrixMarket->Elements, Length);

    return 0;
}

#endif // CONVERTER_CSR_H
