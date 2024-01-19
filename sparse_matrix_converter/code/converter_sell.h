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
    u32 *Columns;
    u32 *SliceOffset;
    void *Elements;

    matrix_market_header Header;

    u32 SliceSize;
    u32 NumberOfSlices;

    u32 AllocateSize;
};

u32
SELL_ConvertFromMatrixMarket(sell *SELL, matrix_market *MatrixMarket)
{
    if(SELL->SliceSize == 0)
        SELL->SliceSize = 2;

    SELL->Header.NumberOfRows = MatrixMarket->Header.NumberOfRows;
    // NOTE(miha): This calculates maximum number of slices we can have.
    if(SELL->Header.NumberOfRows % SELL->SliceSize == 0)
        SELL->NumberOfSlices = SELL->Header.NumberOfRows/SELL->SliceSize;
    else
        SELL->NumberOfSlices = (SELL->Header.NumberOfRows/SELL->SliceSize)+1;
    SELL->AllocateSize = 0;
    SELL->Header.NonZeroElements = MatrixMarket->Header.NonZeroElements;
    SELL->Header.FieldSize = MatrixMarket->Header.FieldSize;
    SELL->Header.NumberOfRows = MatrixMarket->Header.NumberOfRows;
    SELL->Header.NumberOfColumns = MatrixMarket->Header.NumberOfColumns;
    SELL->Header.NonZeroElements = MatrixMarket->Header.NonZeroElements;
    SELL->Header.Object = MatrixMarket->Header.Object;
    SELL->Header.Format = MatrixMarket->Header.Format;
    SELL->Header.Field = MatrixMarket->Header.Field;

    u32 *Max = (u32 *)malloc(SELL->NumberOfSlices * MatrixMarket->Header.FieldSize);
    Memset(Max, 0, SELL->NumberOfSlices * MatrixMarket->Header.FieldSize);

    u32 *ElementsInRow = (u32 *)malloc(MatrixMarket->Header.NumberOfRows * MatrixMarket->Header.FieldSize);
    Memset(ElementsInRow, 0, MatrixMarket->Header.NumberOfRows * MatrixMarket->Header.FieldSize);

    // NOTE(miha): Find length of each row.
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        u32 Row = MatrixMarket->Rows[I];
        ElementsInRow[Row]++;
    }

    // NOTE(miha): Find length of each slice.
    for(u32 Row = 0; Row < MatrixMarket->Header.NumberOfRows; ++Row)
    {
        u32 SliceIndex = Row/SELL->SliceSize;
        if(ElementsInRow[Row] > Max[SliceIndex])
            Max[SliceIndex] = ElementsInRow[Row];
    }

    SELL->SliceOffset = (u32 *)malloc((SELL->NumberOfSlices+1)*sizeof(u32));
    if(SELL->SliceOffset == NULL)
    {
        return MALLOC_ERR;
    }

    // NOTE(miha): Calculate size of the arrays to allocate.
    for(u32 I = 0; I < SELL->NumberOfSlices; ++I)
    {
        SELL->SliceOffset[I] = SELL->AllocateSize;
        printf("so: %d\n", SELL->SliceOffset[I]);
        SELL->AllocateSize += Max[I]*SELL->SliceSize;
    }
    SELL->SliceOffset[SELL->NumberOfSlices] = SELL->AllocateSize;
    printf("so: %d\n", SELL->SliceOffset[SELL->NumberOfSlices]);

    SELL->Elements = malloc(SELL->AllocateSize * SELL->Header.FieldSize);
    if(SELL->Elements == NULL)
    {
        return MALLOC_ERR;
    }
    SELL->Columns = (u32 *)malloc(SELL->AllocateSize * sizeof(u32));
    if(SELL->Columns == NULL)
    {
        return MALLOC_ERR;
    }

    // NOTE(miha): Put elements&columns from matrix market to our SELL structure (correctly padded).
    u32 Pad = 0;
    u32 Index = 0;
    for(u32 R = 0; R < MatrixMarket->Header.NumberOfRows; ++R)
    {
        u32 ElementsPerRow = Max[R/SELL->SliceSize];
        for(u32 C = 0; C < ElementsPerRow; ++C)
        {
            u32 A = SELL->Columns[0];
            u32 B = SELL->Columns[1];
            if(R == MatrixMarket->Rows[Index])
            {
                MM_ToArray(SELL->Elements, f32)[Index+Pad] = MM_ToArray(MatrixMarket->Elements, f32)[Index];
                SELL->Columns[Index+Pad] = MatrixMarket->Columns[Index];
                Index++;
            }
            else
            {
                // NOTE(miha): Pad row with zeros.
                MM_ToArray(SELL->Elements, f32)[Index+Pad] = 0.0f;
                SELL->Columns[Index+Pad] = 0;
                Pad++;
            }
        }
    }

    return 0;
}

#endif // CONVERTER_SELL_H
