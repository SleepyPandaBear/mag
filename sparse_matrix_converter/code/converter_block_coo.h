/*********************************************
* File - converter_block_coo.h
* Author - Miha
* Created - 15 jan 2024
* Description - 
* *******************************************/
#include "converter_read_matrix_market.h"

#if !defined(CONVERTER_BLOCK_COO_H)
#define CONVERTER_BLOCK_COO_H

struct bcoo_block
{
    /* // NOTE(miha): We use column+(row*width) as index. */
    /* u32 Index; */
    u32 Row;
    u32 Column;

    void *Elements;
};


// NOTE(miha): Format COO is the same as read MatrixMarket format.
struct bcoo
{
    matrix_market_header Header;

    bcoo_block *Blocks;

    u32 BlockWidth;
    u32 BlockHeight;
};

internal inline u32
BCOO_ConvertFromMatrixMarket(bcoo *BCOO, matrix_market *MatrixMarket)
{
    u32 BlockLength = 0;

    // alloc max possible blocks
    // iterate MM, and put element to its block
    // iterate again, if block has 0

    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        u32 Row = MM_ToArray(MatrixMarket->Rows, u32)[I];
        u32 Column = MM_ToArray(MatrixMarket->Columns, u32)[I];
        void *Element;
        if(MatrixMarket->Header.Field == REAL)
        {
        }
        else if(MatrixMarket->Header.Field == DOUBLE)
        {
        }
        else if(MatrixMarket->Header.Field == INTEGER)
        {
        }
        else if(MatrixMarket->Header.Field == COMPLEX)
        {
        }
        else if(MatrixMarket->Header.Field == PATTERN)
        {
        }
    }


    return 0;
}

#endif // CONVERTER_BLOCK_COO_H
