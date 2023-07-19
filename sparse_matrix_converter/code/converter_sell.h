/*********************************************
* File - converter_sell.h
* Author - Miha
* Created - 17 jul 2023
* Description - 
* *******************************************/
#if !defined(CONVERTER_SELL_H)
#define CONVERTER_SELL_H

#include "converter_main.h"
#include "converter_read_matrix_market.h"

struct sell
{
    void *Columns;
    void *SliceOffset;
    void *Elements;

    matrix_market_object Object;
    matrix_market_format Format;
    matrix_market_field Field;
    matrix_market_symmetry Symmetry;

    u32 FieldSize;

    u32 NumberOfRows;
    u32 NumberOfColumns;
    u32 NonZeroElements;
    u32 SliceSize;
    u32 NumberOfSlices;
};

void
SELL_ConvertFromMatrixMarket(sell *SELL, matrix_market *MatrixMarket)
{
    if(SELL->SliceSize == 0)
        SELL->SliceSize = 2;
    SELL->NumberOfRows = MatrixMarket->Header.NumberOfRows;
    // NOTE(miha): This calculates maximum number of slices we can have.
    SELL->NumberOfSlices = ((SELL->NumberOfRows-1)/SELL->SliceSize)+1;
    u32 *Max = (u32 *)malloc(SELL->NumberOfSlices * MatrixMarket->Header.FieldSize);
    Memset(Max, 0, SELL->NumberOfSlices * MatrixMarket->Header.FieldSize);

    u32 PreviousRow = 0;
    u32 ElementsInRow = 0;
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        f32 Element = MM_ToArray(MatrixMarket->Elements, f32)[I];
        u32 CurrentRow = MatrixMarket->Rows[I];

        if(PreviousRow == CurrentRow)
        {
            ElementsInRow++;
        }
        else
        {
            if(ElementsInRow > Max[CurrentRow/SELL->NumberOfSlices])
            {
                Max[CurrentRow/SELL->NumberOfSlices] = ElementsInRow;
            }
            ElementsInRow = 1;
            PreviousRow = CurrentRow;
        }
    }
    if(SELL->NumberOfRows % SELL->NumberOfSlices != 0)
        Max[SELL->NumberOfSlices-1] = ElementsInRow;

    printf("number of slices %d\n", SELL->NumberOfSlices);
    for(u32 I = 0; I < SELL->NumberOfSlices; ++I)
        printf("%d\n", Max[I]);

#if 0
    SELL->NumberOfRows = MatrixMarket->Header.NumberOfRows;
    //SELL->NumberOfColumns = Max;
    SELL->FieldSize = MatrixMarket->Header.FieldSize;

    SELL->Columns = malloc(SELL->NumberOfRows * SELL->NumberOfColumns * SELL->FieldSize);
    if(SELL->Columns == NULL)
    {
        return MALLOC_ERR;
    }
    SELL->Elements = malloc(SELL->NumberOfRows * SELL->NumberOfColumns * SELL->FieldSize);
    if(SELL->Elements == NULL)
    {
        return MALLOC_ERR;
    }
    SELL->SliceOffset = malloc(SELL->NumberOfSlices * SELL->FieldSize);
    if(SELL->SliceOffset == NULL)
    {
        return MALLOC_ERR;
    }
#endif
}

#endif // CONVERTER_SELL_H
