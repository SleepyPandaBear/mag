/*********************************************
* File - converter_ell.h
* Author - Miha
* Created - 05 jul 2023
* Description - 
* *******************************************/
#include "converter_read_matrix_market.h"

#if !defined(CONVERTER_ELL_H)
#define CONVERTER_ELL_H

struct ell
{
    u32 *Columns;
    void *Elements;
    u32 ElementsPerRow;

    matrix_market_header Header;
};

internal inline u32
ELL_ConvertFromMatrixMarket(ell *ELL, matrix_market *MatrixMarket)
{
    ELL->Header = MatrixMarket->Header;
    matrix_market_header *Header = &ELL->Header;

    // NOTE(miha): Find max column width.
    // WARN(miha): Assumes that MatrixMarket is sorted by rows.
    u32 Max = 0;
    u32 Prev = 0;
    for(u32 I = 1; I < Header->NonZeroElements; ++I)
    {
        u32 PrevRowEl = ((u32 *)MatrixMarket->Rows)[Prev];
        u32 CurrRowEl = ((u32 *)MatrixMarket->Rows)[I];
        // NOTE(miha): Row value is different.
        if(CurrRowEl != PrevRowEl)
        {
            if((I-Prev) > Max)
                Max = I - Prev;
            Prev = I;
        }
    }
    if((Header->NonZeroElements-Prev) > Max)
        Max = Header->NonZeroElements-Prev;

    ELL->ElementsPerRow = Max;

    ELL->Columns = (u32 *)malloc(Header->NumberOfRows * ELL->ElementsPerRow * Header->FieldSize);
    if(ELL->Columns == NULL)
        return MALLOC_ERR;

    ELL->Elements = malloc(Header->NumberOfRows * ELL->ElementsPerRow * Header->FieldSize);
    if(ELL->Elements == NULL)
        return MALLOC_ERR;

    // NOTE(miha): Create Elements and Columns arrays. If Element exists insert
    // it otherwise fill row with zeros.
    u32 Pad = 0;
    for(u32 R = 0; R < Header->NumberOfRows; ++R)
    {
        for(u32 C = 0; C < ELL->ElementsPerRow; ++C)
        {
            if(R == MatrixMarket->Rows[R*ELL->ElementsPerRow + C - Pad])
            {
                MM_ToArray(ELL->Elements, f32)[R*ELL->ElementsPerRow + C] = MM_ToArray(MatrixMarket->Elements, f32)[R*ELL->ElementsPerRow + C - Pad];
                ELL->Columns[R*ELL->ElementsPerRow + C] = MatrixMarket->Columns[R*ELL->ElementsPerRow+C-Pad];
            }
            else
            {
                // NOTE(miha): Pad row with zeros.
                MM_ToArray(ELL->Elements, f32)[R*ELL->ElementsPerRow + C] = 0.0f;
                ELL->Columns[R*ELL->ElementsPerRow + C] = 0;
                Pad++;
            }
        }
    }

    return 0;
}

#endif // CONVERTER_ELL_H
