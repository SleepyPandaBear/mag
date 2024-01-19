/*********************************************
* File - converter_bcsr.h
* Author - Miha
* Created - 31 jul 2023
* Description - 
* *******************************************/
#include "converter_read_matrix_market.h"
#include "stdlib.h"
#include <cstdlib>

#if !defined(CONVERTER_BCSR_H)
#define CONVERTER_BCSR_H

struct bcsr
{
    u32 *RowPointers;
    u32 *Columns;
    void *Elements; // TODO(miha): This is probably bcsr_block *

    u32 BlockSize;

    matrix_market_header Header;
};

// TODO(miha): Maybe find a better way?
struct bcsr_block
{
    u32 Row;
    u32 Column;
    // TODO(miha): Should we use NumberOfElements, Elements, ElementCount? This apply for all files.
    u32 ElementCount;
};

int 
BCSR_BlockCompare(const void* P1, const void* P2)
{
    bcsr_block B1 = *(bcsr_block *)P1;
    bcsr_block B2 = *(bcsr_block *)P2;

    if(B1.Row < B2.Row) return -1;
    else if(B1.Row > B2.Row) return 1;
    else
    {
        if(B1.Column < B2.Column) return -1;
        else if(B1.Column > B2.Column) return 1;
    }

    return 0;
}

u32
BCSR_ConvertFromMatrixMarket(bcsr *BCSR, matrix_market *MatrixMarket)
{
    if(BCSR->BlockSize == 0)
        BCSR->BlockSize = 2;

    bcsr_block *Blocks = (bcsr_block *)malloc(MatrixMarket->Header.NonZeroElements * sizeof(bcsr_block));
    Memset(Blocks, 0, MatrixMarket->Header.NonZeroElements*sizeof(bcsr_block));

    // NOTE(miha): First pass. 
    u32 BlocksCount = 0;
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        u32 Row = MatrixMarket->Rows[I];
        u32 Column = MatrixMarket->Columns[I];
        u32 RowIndex = Row/BCSR->BlockSize;
        u32 ColumnIndex = Column/BCSR->BlockSize;

        for(u32 BlockIndex = 0; BlockIndex < MatrixMarket->Header.NonZeroElements; ++BlockIndex)
        {
            // NOTE(miha): There is no block for current element so init new block.
            if(Blocks[BlockIndex].ElementCount == 0)
            {
                BlocksCount++;
                Blocks[BlockIndex].Row = RowIndex;
                Blocks[BlockIndex].Column = ColumnIndex;
                Blocks[BlockIndex].ElementCount++;
                break;
            }

            if(Blocks[BlockIndex].Row == RowIndex && Blocks[BlockIndex].Column == ColumnIndex)
            {
                Blocks[BlockIndex].ElementCount++;
                break;
            }
        }
    }

    qsort(&Blocks[0], BlocksCount, sizeof(bcsr_block), BCSR_BlockCompare);

    u32 RowPointersSize = Ceil((f32)MatrixMarket->Header.NumberOfRows/(f32)BCSR->BlockSize);
    u32 *Histogram = (u32 *)malloc(RowPointersSize*sizeof(u32));
    Memset(Histogram, 0, RowPointersSize*sizeof(u32));
    for(u32 I = 0; I < BlocksCount; ++I)
    {
        Histogram[Blocks[I].Row]++;
    }

    BCSR->RowPointers = (u32 *)malloc((RowPointersSize+1)*sizeof(u32));
    if(BCSR->RowPointers == NULL)
        return MALLOC_ERR;
    Memset(BCSR->RowPointers, 0, (RowPointersSize+1)*sizeof(u32));
    for(u32 I = 1; I < RowPointersSize+1; ++I)
    {
        BCSR->RowPointers[I] += BCSR->RowPointers[I-1]+Histogram[I-1];
    }

    BCSR->Columns = (u32 *)malloc(BlocksCount*sizeof(u32));
    if(BCSR->Columns == NULL)
        return MALLOC_ERR;
    Memset(BCSR->Columns, 0, BlocksCount*sizeof(u32));
    for(u32 I = 0; I < BlocksCount; ++I)
    {
        BCSR->Columns[I] = Blocks[I].Column;
    }

    BCSR->Elements = malloc(BlocksCount*BCSR->BlockSize*BCSR->BlockSize*MatrixMarket->Header.FieldSize);
    if(BCSR->Elements == NULL)
        return MALLOC_ERR;
    Memset(BCSR->Elements, 0, BlocksCount*BCSR->BlockSize*BCSR->BlockSize*MatrixMarket->Header.FieldSize);
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        u32 Row = MatrixMarket->Rows[I];
        u32 Column = MatrixMarket->Columns[I];
        u32 RowIndex = Row/BCSR->BlockSize;
        u32 ColumnIndex = Column/BCSR->BlockSize;

        for(u32 BlockIndex = 0; BlockIndex < BlocksCount; ++BlockIndex)
        {
            if(Blocks[BlockIndex].Row == RowIndex && Blocks[BlockIndex].Column == ColumnIndex)
            {
                MM_ToArray(BCSR->Elements, f32)[BlockIndex*BCSR->BlockSize*BCSR->BlockSize+((Row%BCSR->BlockSize)*BCSR->BlockSize + (Column%BCSR->BlockSize))] = MM_ToArray(MatrixMarket->Elements, f32)[I];
                break;
            }
        }
    }

    BCSR->Header = MatrixMarket->Header;

    return 0;
}

#endif // CONVERTER_BCSR_H
