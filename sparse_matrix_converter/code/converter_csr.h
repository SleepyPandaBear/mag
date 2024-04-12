/*********************************************
 * File - converter_csr.cpp
 * Author - Miha
 * Created - 04 jul 2023
 * Description -
 * *******************************************/
#include "converter_read_matrix_market.h"

#if !defined(CONVERTER_CSR_H)
#define CONVERTER_CSR_H

struct csr
{
    void *RowPointers;
    void *Columns;
    void *Elements;

    matrix_market_header Header;
};

struct csr_fgpa
{
    u32 *RowPointers;
    u32 *Columns;
    dtype *Elements;

    matrix_market_header Header;
};

internal inline u32
CSR_ConvertFromMatrixMarket(csr *CSR, matrix_market *MatrixMarket)
{
    CSR->Header = MatrixMarket->Header;
    matrix_market_header *Header = &CSR->Header;

    CSR->RowPointers = malloc((Header->NumberOfRows + 1) * Header->FieldSize);
    if(CSR->RowPointers == NULL)
        return MALLOC_ERR;
    Memset(CSR->RowPointers, 0, (Header->NumberOfRows + 1) * Header->FieldSize);

    CSR->Columns = malloc(Header->NonZeroElements * Header->FieldSize);
    if(CSR->Columns == NULL)
        return MALLOC_ERR;

    CSR->Elements = malloc(Header->NonZeroElements * Header->FieldSize);
    if(CSR->Elements == NULL)
        return MALLOC_ERR;

    for(u32 I = 0; I < Header->NonZeroElements; ++I)
    {
        if(Header->FieldSize == 4)
        {
            MM_ToArray(CSR->RowPointers, u32)[MM_ToArray(MatrixMarket->Rows, u32)[I] + 1]++;
        }
        else if(MatrixMarket->Header.FieldSize == 8)
        {
            u64 Element = ((u64 *)MatrixMarket->Elements)[I];
        }
        else
        {
            return UNKNOWN_VALUE_ERR;
        }
    }

    for(u32 I = 1; I < Header->NumberOfRows + 1; ++I)
        MM_ToArray(CSR->RowPointers, u32)[I] += MM_ToArray(CSR->RowPointers, u32)[I - 1];

    u32 Length = Header->NonZeroElements * Header->FieldSize;
    CopyMemory(CSR->Columns, MatrixMarket->Columns, Length);
    CopyMemory(CSR->Elements, MatrixMarket->Elements, Length);

    return 0;
}

internal inline u32
CSR_ConvertToFPGA(csr *CSR, csr_fgpa *CSR_FPGA)
{
#if !defined(dtype)
    printf("Please define 'dtype'\n");
    return 128;
#endif

    CSR_FPGA->Header = CSR->Header;

    matrix_market_header *Header = &CSR->Header;

    CSR_FPGA->RowPointers = (u32 *)malloc((Header->NumberOfRows + 1) * sizeof(u32));
    if(CSR_FPGA->RowPointers == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < Header->NumberOfRows + 1; ++I)
        CSR_FPGA->RowPointers[I] = MM_ToArray(CSR->RowPointers, u32)[I];

    CSR_FPGA->Columns = (u32 *)malloc(Header->NonZeroElements * sizeof(u32));
    if(CSR_FPGA->Columns == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < Header->NonZeroElements; ++I)
        CSR_FPGA->Columns[I] = MM_ToArray(CSR->Columns, u32)[I];

    CSR_FPGA->Elements = (dtype *)malloc(Header->NonZeroElements * sizeof(dtype));
    if(CSR_FPGA->Elements == NULL)
        return MALLOC_ERR;
    for(i32 I = 0; I < Header->NonZeroElements; ++I)
        CSR_FPGA->Elements[I] = MM_ToArray(CSR->Elements, dtype)[I];

    return 0;
}

#endif // CONVERTER_CSR_H
