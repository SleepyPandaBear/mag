/*********************************************
* File - converter_coo.h
* Author - Miha
* Created - 01 jul 2023
* Description - 
* *******************************************/
#include "converter_read_matrix_market.h"

#if !defined(CONVERTER_COO_H)
#define CONVERTER_COO_H

// NOTE(miha): Format COO is the same as read MatrixMarket format.
struct coo
{
    void *Rows;
    void *Columns;
    void *Elements;

    matrix_market_header Header;
};

internal inline u32
COO_ConvertFromMatrixMarket(coo *COO, matrix_market *MatrixMarket)
{
    COO->Header = MatrixMarket->Header;
    matrix_market_header *Header = &COO->Header;

    COO->Rows = malloc(Header->NonZeroElements * Header->FieldSize);
    if(COO->Rows == NULL)
        return MALLOC_ERR;
    COO->Columns = malloc(Header->NonZeroElements * Header->FieldSize);
    if(COO->Columns == NULL)
        return MALLOC_ERR;
    COO->Elements = malloc(Header->NonZeroElements * Header->FieldSize);
    if(COO->Elements == NULL)
        return MALLOC_ERR;

    u32 Length = Header->NonZeroElements * Header->FieldSize;
    CopyMemory(COO->Rows, MatrixMarket->Rows, Length);
    CopyMemory(COO->Columns, MatrixMarket->Columns, Length);
    CopyMemory(COO->Elements, MatrixMarket->Elements, Length);

    return 0;
}

#endif // CONVERTER_COO_H
