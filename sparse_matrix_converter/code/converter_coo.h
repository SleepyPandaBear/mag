/*********************************************
* File - converter_coo.h
* Author - Miha
* Created - 01 jul 2023
* Description - 
* *******************************************/
#if !defined(CONVERTER_COO_H)
#define CONVERTER_COO_H

#include "converter_read_matrix_market.h"

// NOTE(miha): Format COO is the same as read MatrixMarket format.
struct coo
{
    void *Rows;
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
COO_ConvertFromMatrixMarket(coo *COO, matrix_market *MatrixMarket)
{
    COO->Rows = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(COO->Rows == NULL)
    {
        return MALLOC_ERR;
    }
    COO->Columns = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(COO->Columns == NULL)
    {
        return MALLOC_ERR;
    }
    COO->Elements = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(COO->Elements == NULL)
    {
        return MALLOC_ERR;
    }

    u32 Length = MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize;
    CopyMemory(COO->Rows, MatrixMarket->Rows, Length);
    CopyMemory(COO->Columns, MatrixMarket->Columns, Length);
    CopyMemory(COO->Elements, MatrixMarket->Elements, Length);

    COO->FieldSize = MatrixMarket->Header.FieldSize;
    COO->NumberOfRows = MatrixMarket->Header.NumberOfRows;
    COO->NumberOfColumns = MatrixMarket->Header.NumberOfColumns;
    COO->NonZeroElements = MatrixMarket->Header.NonZeroElements;

    COO->Object = MatrixMarket->Header.Object;
    COO->Format = MatrixMarket->Header.Format;
    COO->Field = MatrixMarket->Header.Field;
    COO->Symmetry = MatrixMarket->Header.Symmetry;

    return 0;
}

#endif // CONVERTER_COO_H
