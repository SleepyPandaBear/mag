/*********************************************
 * File - converter_coo.h
 * Author - Miha
 * Created - 01 jul 2023
 * Description -
 * *******************************************/
#if !defined(CONVERTER_COO_H)
#define CONVERTER_COO_H

#include "converter_main.h"
#include "converter_read_matrix_market.h"

// NOTE(miha): Format COO is the same as read MatrixMarket format.
struct coo
{
    void *Rows;
    void *Columns;
    void *Elements;

    matrix_market_header Header;
};

struct coo_fpga
{
    u32 *Rows;
    u32 *Columns;
    dtype *Elements;

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

internal inline u32
COO_ConvertToFPGA(coo *COO, coo_fpga *COO_FPGA)
{
#if !defined(dtype)
    printf("Please define 'dtype'\n");
    return 128;
#endif

    COO_FPGA->Header = COO->Header;

    matrix_market_header *Header = &COO->Header;

    COO_FPGA->Rows = (u32 *)malloc(Header->NonZeroElements * sizeof(u32));
    if(COO_FPGA->Rows == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < Header->NumberOfRows; ++I)
        COO_FPGA->Rows[I] = MM_ToArray(COO->Rows, u32)[I];

    COO_FPGA->Columns = (u32 *)malloc(Header->NonZeroElements * sizeof(u32));
    if(COO_FPGA->Columns == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < Header->NumberOfColumns; ++I)
        COO_FPGA->Columns[I] = MM_ToArray(COO->Columns, u32)[I];

    COO_FPGA->Elements = (dtype *)malloc(Header->NonZeroElements * Header->FieldSize);
    if(COO_FPGA->Elements == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < Header->NonZeroElements; ++I)
        COO_FPGA->Elements[I] = MM_ToArray(COO->Elements, dtype)[I];

    return 0;
}

#endif // CONVERTER_COO_H
