/*********************************************
 * File - converter_jad.h
 * Author - Miha
 * Created - 01 aug 2023
 * Description -
 * *******************************************/
#include "converter_read_matrix_market.h"
#include "stdlib.h"

#include <cstdlib>

#if !defined(CONVERTER_JAD_H)
#define CONVERTER_JAD_H

struct jad
{
    u32 *RowPointers;
    u32 *Columns;
    u32 *Permutations;
    void *Elements;

    matrix_market_header Header;
};

struct jad_row
{
    u32 Row;
    u32 Count;
};

struct jad_fpga
{
    u32 *RowPointers;
    u32 *Columns;
    u32 *Permutations;
    dtype *Elements;

    matrix_market_header Header;
};

// TODO(miha): Should we use JAD block? Block holds: row and nnz?
static inline int
JAD_Compare(const void *P1, const void *P2)
{
    u32 N1 = ((jad_row *)P1)->Count;
    u32 N2 = ((jad_row *)P2)->Count;

    if(N1 > N2)
        return -1;
    else if(N1 < N2)
        return 1;
    else
        return 0;
}

// TODO(miha): Free every allocated memory (eg. in BCSR converter we never free some memory).

static inline u32
JAD_ConvertFromMatrixMarket(jad *JAD, matrix_market *MatrixMarket)
{
    JAD->Header = MatrixMarket->Header;
    jad_row *ElementsInRow = (jad_row *)malloc(MatrixMarket->Header.NumberOfRows * sizeof(jad_row));
    if(ElementsInRow == NULL)
        return MALLOC_ERR;
    Memset(ElementsInRow, 0, MatrixMarket->Header.NumberOfRows * sizeof(jad_row));

    JAD->Permutations = (u32 *)malloc(MatrixMarket->Header.NumberOfRows * sizeof(u32));
    if(JAD->Permutations == NULL)
        return MALLOC_ERR;
    Memset(JAD->Permutations, 0, MatrixMarket->Header.NumberOfRows * sizeof(u32));

    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        u32 Row = MatrixMarket->Rows[I];
        ElementsInRow[Row].Row = Row;
        ElementsInRow[Row].Count++;
    }

    u32 *Cumulative = (u32 *)malloc(MatrixMarket->Header.NumberOfRows * sizeof(u32));
    if(Cumulative == NULL)
        return MALLOC_ERR;
    for(u32 I = 1; I < MatrixMarket->Header.NumberOfRows; ++I)
    {
        Cumulative[I] += Cumulative[I - 1] + ElementsInRow[I - 1].Count;
        ElementsInRow[I].Row = I;
    }

    qsort(ElementsInRow, MatrixMarket->Header.NumberOfRows, sizeof(jad_row), JAD_Compare);

    for(u32 I = 0; I < MatrixMarket->Header.NumberOfRows; ++I)
    {
        JAD->Permutations[I] = ElementsInRow[I].Row;
    }

    // TODO
    JAD->RowPointers = (u32 *)malloc((ElementsInRow[0].Count + 1) * sizeof(u32));
    if(JAD->RowPointers == NULL)
        return MALLOC_ERR;
    JAD->Columns = (u32 *)malloc(MatrixMarket->Header.NonZeroElements * sizeof(u32));
    if(JAD->Columns == NULL)
        return MALLOC_ERR;
    JAD->Elements = malloc(MatrixMarket->Header.NonZeroElements * MatrixMarket->Header.FieldSize);
    if(JAD->Elements == NULL)
        return MALLOC_ERR;

    JAD->RowPointers[0] = 0;
    u32 ElementIndex = 0;
    for(u32 ColumnIndex = 0; ColumnIndex < ElementsInRow[0].Count; ++ColumnIndex)
    {
        for(u32 RowIndex = 0; RowIndex < MatrixMarket->Header.NumberOfRows; ++RowIndex)
        {
            if(ColumnIndex < ElementsInRow[RowIndex].Count)
            {
                f32 Element = MM_ToArray(MatrixMarket->Elements, f32)[Cumulative[JAD->Permutations[RowIndex]] + ColumnIndex];
                u32 Column = MatrixMarket->Columns[Cumulative[JAD->Permutations[RowIndex]] + ColumnIndex];
                MM_ToArray(JAD->Elements, f32)[ElementIndex] = Element;
                JAD->Columns[ElementIndex] = Column;
                ElementIndex++;
            }
        }
        JAD->RowPointers[ColumnIndex + 1] = ElementIndex;
    }

    for(u32 I = 0; I < MatrixMarket->Header.NumberOfRows; ++I)
        printf("%d, ", JAD->Permutations[I]);
    printf("\n");

    for(u32 I = 0; I < ElementsInRow[0].Count + 1; ++I)
        printf("%d, ", JAD->RowPointers[I]);
    printf("\n");

    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
        printf("%d, ", JAD->Columns[I]);
    printf("\n");

    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
        printf("%f, ", MM_ToArray(JAD->Elements, f32)[I]);
    printf("\n");

    return 0;
}

internal inline u32
JAD_ConvertToFPGA(jad *JAD, jad_fpga *JAD_FPGA)
{
#if !defined(dtype)
    printf("Please define 'dtype'\n");
    return 128;
#endif

    JAD_FPGA->Header = JAD->Header;

    matrix_market_header *Header = &JAD->Header;

    JAD_FPGA->Offsets = (i32 *)malloc(JAD->NumberOfDiagonals * sizeof(i32));
    if(JAD_FPGA->Offsets == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < JAD->NumberOfDiagonals; ++I)
        JAD_FPGA->Offsets[I] = MM_ToArray(JAD->Offsets, u32)[I];

    JAD_FPGA->Elements = (dtype *)malloc(JAD->NumberOfDiagonals * JAD->MainDiagonalLength * sizeof(dtype));
    if(JAD_FPGA->Elements == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < JAD->NumberOfDiagonals * JAD->MainDiagonalLength; ++I)
        JAD_FPGA->Elements[I] = MM_ToArray(JAD->Elements, dtype)[I];

    JAD_FPGA->NumberOfDiagonals = JAD->NumberOfDiagonals;
    JAD_FPGA->MainDiagonalLength = JAD->MainDiagonalLength;

    return 0;
}

#endif // CONVERTER_JAD_H
