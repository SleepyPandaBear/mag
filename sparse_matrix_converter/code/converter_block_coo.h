/*********************************************
 * File - converter_block_coo.h
 * Author - Miha
 * Created - 15 jan 2024
 * Description -
 * *******************************************/
#include "converter_read_matrix_market.h"
#include "stdlib.h"

#if !defined(CONVERTER_BLOCK_COO_H)
#define CONVERTER_BLOCK_COO_H

struct bcoo_block {
    /* // NOTE(miha): We use column+(row*width) as index. */
    /* u32 Index; */
    u32 Row;
    u32 Column;

    void *Elements;
};

// NOTE(miha): Format COO is the same as read MatrixMarket format.
struct bcoo {
    matrix_market_header Header;

    bcoo_block *Blocks;
    u32 BlockLength;

    u32 BlockWidth;
    u32 BlockHeight;
};

internal inline 
i32
FindBlockIndex(bcoo *BCOO, u32 Row, u32 Column)
{
    u32 BlockRow = Row / BCOO->BlockWidth;
    u32 BlockColumn = Column / BCOO->BlockHeight;
    for(u32 I = 0; I < BCOO->BlockLength; ++I)
    {
        bcoo_block Block = BCOO->Blocks[I];
        if(Block.Row == BlockRow && Block.Column == BlockColumn)
        {
            return I;
        }
    }

    return -1;
}

internal inline
i32
CreateBlock(bcoo *BCOO, u32 Row, u32 Column)
{    
    u32 BlockRow = Row / BCOO->BlockWidth;
    u32 BlockColumn = Column / BCOO->BlockHeight;

    if(BCOO->BlockLength == 0)
    {
        BCOO->Blocks = (bcoo_block *)malloc(sizeof(bcoo_block));
        BCOO->Blocks[0].Elements = malloc(BCOO->BlockWidth * BCOO->BlockHeight * BCOO->Header.FieldSize);
        BCOO->Blocks[0].Row = BlockRow;
        BCOO->Blocks[0].Column = BlockColumn;
        BCOO->BlockLength = 1;
    }
    else
    {
        BCOO->BlockLength += 1;
        BCOO->Blocks = (bcoo_block *)realloc(BCOO->Blocks, BCOO->BlockLength * sizeof(bcoo_block));
        BCOO->Blocks[BCOO->BlockLength-1].Elements = malloc(BCOO->BlockWidth * BCOO->BlockHeight * BCOO->Header.FieldSize);
        BCOO->Blocks[BCOO->BlockLength-1].Row = BlockRow;
        BCOO->Blocks[BCOO->BlockLength-1].Column = BlockColumn;
    }

    return BCOO->BlockLength-1;
}

internal inline
i32
FindElementIndex(bcoo *BCOO, u32 Row, u32 Column)
{    
    u32 ElementRow = Row % BCOO->BlockWidth;
    u32 ElementColumn = Column % BCOO->BlockHeight;
    
    return ElementColumn + (ElementRow * BCOO->BlockWidth);
}

int 
BCOO_BlockCompare(const void* P1, const void* P2)
{
    bcoo_block B1 = *(bcoo_block *)P1;
    bcoo_block B2 = *(bcoo_block *)P2;

    if(B1.Row < B2.Row) return -1;
    else if(B1.Row > B2.Row) return 1;
    else
    {
        if(B1.Column < B2.Column) return -1;
        else if(B1.Column > B2.Column) return 1;
    }

    return 0;
}

internal inline 
u32 
BCOO_ConvertFromMatrixMarket(bcoo *BCOO, matrix_market *MatrixMarket) 
{

    // alloc max possible blocks
    // iterate MM, and put element to its block
    // iterate again, if block has 0

    BCOO->Header = MatrixMarket->Header;

    u32 Last = 0;
    for (u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I) {
        u32 Row = MM_ToArray(MatrixMarket->Rows, u32)[I];
        u32 Column = MM_ToArray(MatrixMarket->Columns, u32)[I];
        i32 BlockIndex = FindBlockIndex(BCOO, Row, Column);
        if(BlockIndex == -1)
            BlockIndex = CreateBlock(BCOO, Row, Column);
        i32 ElementIndex = FindElementIndex(BCOO, Row, Column);
        if (MatrixMarket->Header.Field == REAL) {
        } else if (MatrixMarket->Header.Field == DOUBLE) {
        } else if (MatrixMarket->Header.Field == INTEGER) {
        } else if (MatrixMarket->Header.Field == COMPLEX) {
        } else if (MatrixMarket->Header.Field == PATTERN) {
            u32 Element = MM_ToArray(MatrixMarket->Elements, u32)[I]; 
            MM_ToArray(BCOO->Blocks[BlockIndex].Elements, u32)[ElementIndex] = Element;
        }
    }

    qsort(&BCOO->Blocks[0], BCOO->BlockLength, sizeof(bcoo_block), BCOO_BlockCompare);

    return 0;
}

void
BCOO_Print(bcoo *BCOO)
{
    for(u32 BlockIndex = 0; BlockIndex < BCOO->BlockLength; ++BlockIndex)
    {
        bcoo_block Block = BCOO->Blocks[BlockIndex];
        printf("Block %d, b_row: %d, b_col: %d\n", BlockIndex, Block.Row, Block.Column);
        u32 ElementsInBlock = BCOO->BlockWidth * BCOO->BlockHeight;
        for(u32 ElementIndex = 0; ElementIndex < ElementsInBlock; ++ElementIndex)
        {
            if(BCOO->Header.Field == PATTERN) 
            {
                printf("%d, ", MM_ToArray(Block.Elements, u32)[ElementIndex]);
            }
            if(ElementIndex % BCOO->BlockWidth == BCOO->BlockWidth-1)
                printf("\n");
        }
        printf("\n");
        printf("#################################\n");
    }
}

#endif // CONVERTER_BLOCK_COO_H
