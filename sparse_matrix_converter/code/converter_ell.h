/*********************************************
* File - converter_ell.h
* Author - Miha
* Created - 05 jul 2023
* Description - 
* *******************************************/
#if !defined(CONVERTER_ELL_H)
#define CONVERTER_ELL_H

#include "converter_read_matrix_market.h"

struct ell
{
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
ELL_ConvertFromMatrixMarket(ell *ELL, matrix_market *MatrixMarket)
{
    // NOTE(miha): Find ell columns width;
    u32 Max = 0;
    u32 Prev = 0;
    for(u32 I = 1; I < MatrixMarket->Header.NonZeroElements; ++I)
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
    if((MatrixMarket->Header.NonZeroElements-Prev) > Max)
        Max = MatrixMarket->Header.NonZeroElements-Prev;

    ELL->NumberOfRows = MatrixMarket->Header.NumberOfRows;
    ELL->NumberOfColumns = Max;
    ELL->FieldSize = MatrixMarket->Header.FieldSize;

    ELL->Columns = malloc(ELL->NumberOfRows * ELL->NumberOfColumns * ELL->FieldSize);
    if(ELL->Columns == NULL)
    {
        return MALLOC_ERR;
    }
    ELL->Elements = malloc(ELL->NumberOfRows * ELL->NumberOfColumns * ELL->FieldSize);
    if(ELL->Elements == NULL)
    {
        return MALLOC_ERR;
    }

    Prev = 0;
    u32 CurrentRowLength = 0;
    u32 Pad = 0;
    for(u32 I = 1; I < MatrixMarket->Header.NonZeroElements+1; ++I)
    {
        u32 PrevRowEl = ((u32 *)MatrixMarket->Rows)[Prev];
        u32 CurrRowEl = ((u32 *)MatrixMarket->Rows)[I];

        // f32 CurrElement = ((f32 *)MatrixMarket->Elements)[I-1-Pad];

        ((f32 *)ELL->Elements)[I-1+Pad] = ((f32 *)MatrixMarket->Elements)[I-1];
        ((u32 *)ELL->Columns)[I-1+Pad] = ((u32 *)MatrixMarket->Columns)[I-1];
        CurrentRowLength++;

        if(PrevRowEl != CurrRowEl)
        {
            // NOTE(miha): Pad arrays with zeros.
            while(CurrentRowLength < ELL->NumberOfColumns)
            {
                ((f32 *)ELL->Elements)[I+Pad] = 0;
                ((u32 *)ELL->Columns)[I+Pad] = 0;
                Pad++;
                CurrentRowLength++;
            }
            CurrentRowLength = 0;
        }
        Prev = I;
    }

    ELL->Object = MatrixMarket->Header.Object;
    ELL->Format = MatrixMarket->Header.Format;
    ELL->Field = MatrixMarket->Header.Field;
    ELL->Symmetry = MatrixMarket->Header.Symmetry;

    return 0;
}

#endif // CONVERTER_ELL_H
