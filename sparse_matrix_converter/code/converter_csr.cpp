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

void
CSR_ConvertFromMatrixMarket(csr *CSR, matrix_market *MatrixMarket)
{
    CSR->RowPointers = malloc(MatrixMarket->Header.NumberOfRows * MatrixMarket->Header.FieldSize);
    if(CSR->RowPointers == NULL)
    {
        // TODO(miha): Handle error.
        return;
    }
    CSR->Columns = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(CSR->Columns == NULL)
    {
        // TODO(miha): Handle error.
        return;
    }
    CSR->Elements = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(CSR->Elements == NULL)
    {
        // TODO(miha): Handle error.
        return;
    }

    u32 RowPointersIndex = 0;
    u32 SameElementCount = 0;
    for(u32 I = 0; I < MatrixMarket->Header.NumberOfRows-1; ++I)
    {
        if(MatrixMarket->Header.FieldSize == 4)
        {
            u32 Element = ((u32 *)MatrixMarket->Elements)[I];
            u32 NextElement = ((u32 *)MatrixMarket->Elements)[I+1];

            if(Element == NextElement)
            {
                SameElementCount++;
            }
            else
            {
                ((u32 *)CSR->RowPointers)[RowPointersIndex] = SameElementCount;
                RowPointersIndex++;
                SameElementCount = 0;
            }
        }
        else if(MatrixMarket->Header.FieldSize == 8)
        {
            u64 Element = ((u64 *)MatrixMarket->Elements)[I];
        }
        else
        {
            // TODO(miha): Handle error.
        }
    }

    u32 Length = MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize;
    CopyMemory(COO->Columns, MatrixMarket->Columns, Length);
    CopyMemory(COO->Elements, MatrixMarket->Elements, Length);
}

#endif // CONVERTER_CSR_H
