/*********************************************
 * File - tests.h
 * Author - Miha
 * Created - 12 jul 2023
 * Description -
 * *******************************************/
#if !defined(TESTS_H)
#define TESTS_H

// NOTE(miha): Copied from: https://jera.com/techinfo/jtns/jtn002.
#define mu_str(String) (char *)String
// TODO(miha): Create mu_scope that would print current name of the funciton +
// scope name.
#define mu_scope(Name) printf("Running tests %s in function %s\n", Name, __func__)
#define mu_assert(message, test) do{if(!(test)) return message;} while(0)
#define mu_run_test(test) do{char *message = test(); tests_run++;if(message) return message;} while(0)
extern int tests_run;
int tests_run = 0;

#include "blas.h"
#include "blas_bcsr.h"
#include "blas_coo.h"
#include "blas_csr.h"
#include "blas_dia.h"
#include "blas_ell.h"
#include "blas_jad.h"
#include "blas_sell.h"
#include "converter_bcsr.h"
#include "converter_coo.h"
#include "converter_csr.h"
#include "converter_dia.h"
#include "converter_ell.h"
#include "converter_jad.h"
#include "converter_main.h"
#include "converter_read_matrix_market.h"
#include "converter_sell.h"
#include "math.h"
#include "stdio.h"

#define EPSILON 0.05f

static matrix_market Matrix = {};
static matrix_market MatrixEmptyLine = {};
static matrix_market MatrixBCSR = {};

static char *TestArray_f32(f32 *A, f32 *B, u32 Length, f32 Epsilon) 
{
    for (u32 I = 0; I < Length; ++I) 
    {
        if (fabs(A[I] - B[I]) > Epsilon) 
        {
            char Buffer[256];
            sprintf(Buffer, "%f != %f at index %d\n", A[I], B[I], I);
            printf("%s", Buffer);
            return mu_str("error happended");
        }
    }

    return 0;
}

static char *TestArray_i32(i32 *A, i32 *B, u32 Length) 
{
    for (u32 I = 0; I < Length; ++I) 
    {
        if (A[I] != B[I]) 
        {
            char Buffer[256];
            sprintf(Buffer, "%d != %d at index %d\n", A[I], B[I], I);
            printf("%s", Buffer);
            return mu_str("error happended");
        }
    }

    return 0;
}

static char *TestArray_u32(u32 *A, u32 *B, u32 Length) 
{
    for (u32 I = 0; I < Length; ++I) 
    {
        if (A[I] != B[I]) 
        {
            char Buffer[256];
            sprintf(Buffer, "%d != %d at index %d\n", A[I], B[I], I);
            printf("%s", Buffer);
            return mu_str("error happended");
        }
    }

    return 0;
}

static char *TestMatrixMarketHeader(matrix_market_header *Header, matrix_market *MatrixMarket) 
{
    mu_scope("test header");
    mu_assert(mu_str("Object differs"), Header->Object == MatrixMarket->Header.Object); // object
    mu_assert(mu_str("Format differs"), Header->Format == MatrixMarket->Header.Format); // format
    mu_assert(mu_str("Field differs"), Header->Field == MatrixMarket->Header.Field); // field
    mu_assert(mu_str("Symmetry differs"), Header->Symmetry == MatrixMarket->Header.Symmetry); // symmetry
    mu_assert(mu_str("Fieldsize differs"), Header->FieldSize == MatrixMarket->Header.FieldSize); // fieldsize
    mu_assert(mu_str("Number of rows differs"), Header->NumberOfRows == MatrixMarket->Header.NumberOfRows); // number of rows
    mu_assert(mu_str("Number of columns differs"), Header->NumberOfColumns == MatrixMarket->Header.NumberOfColumns); // number of columns
    mu_assert(mu_str("Number of non zero elements differs"), Header->NonZeroElements == MatrixMarket->Header.NonZeroElements); // number of non zero elements
    return 0;
}

static char *
COO_Tests() 
{
    u32 Error;
    char *ErrorMessage;

    // NOTE(miha): Tests on Matrix.
    {
        mu_scope("Matrix init");
        coo Format = {};
        Error = COO_ConvertFromMatrixMarket(&Format, &Matrix);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &Matrix);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Rows not allocated"), Format.Rows != NULL); // rows
        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectRows[] = {0, 0, 0, 1, 1, 2, 2, 2, 3};
        ErrorMessage = TestArray_u32((u32 *)Format.Rows, &CorrectRows[0], Format.Header.NonZeroElements);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectColumns[] = {1, 2, 3, 0, 2, 0, 1, 3, 2};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.Header.NumberOfColumns);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 9.0f, 3.0f, 4.0f,
            5.0f, 6.0f, 8.0f, 7.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.Header.NonZeroElements, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 4;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;

        COO_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {44.0f, 15.0f, 49.0f, 21.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 4, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    {
        mu_scope("matrix empty line");
        coo Format = {};
        Error = COO_ConvertFromMatrixMarket(&Format, &MatrixEmptyLine);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &MatrixEmptyLine);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Rows not allocated"), Format.Rows != NULL); // rows
        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectRows[] = {0, 0, 1, 3, 3, 4, 4, 4};
        ErrorMessage = TestArray_u32((u32 *)Format.Rows, &CorrectRows[0], Format.Header.NonZeroElements);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectColumns[] = {0, 2, 1, 0, 1, 1, 2, 3};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.Header.NumberOfColumns);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.Header.NonZeroElements, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 5;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;
        ((f32 *)Out.Elements)[4] = 0.0f;

        COO_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {7.0f, 6.0f, 0.0f, 14.0f, 65.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 5, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    return 0;
}

static char *
CSR_Tests() 
{
    u32 Error;
    char *ErrorMessage;

    // NOTE(miha): Tests on Matrix.
    {
        mu_scope("Matrix init");
        csr Format = {};
        Error = CSR_ConvertFromMatrixMarket(&Format, &Matrix);
        mu_assert(mu_str("Error converting to Format format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &Matrix);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Rows not allocated"), Format.RowPointers != NULL); // rows
        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectRows[] = {0, 3, 5, 8, 9};
        ErrorMessage = TestArray_u32((u32 *)Format.RowPointers, &CorrectRows[0], Format.Header.NumberOfRows);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectColumns[] = {1, 2, 3, 0, 2, 0, 1, 3, 2};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.Header.NumberOfColumns);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 9.0f, 3.0f, 4.0f, 5.0f, 6.0f, 8.0f, 7.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.Header.NonZeroElements, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 4;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;

        CSR_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {44.0f, 15.0f, 49.0f, 21.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 4, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    {
        mu_scope("matrix empty line");
        csr Format = {};
        Error = CSR_ConvertFromMatrixMarket(&Format, &MatrixEmptyLine);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &MatrixEmptyLine);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Rows not allocated"), Format.RowPointers != NULL); // rows
        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectRows[] = {0, 2, 3, 3, 5, 8};
        ErrorMessage = TestArray_u32((u32 *)Format.RowPointers, &CorrectRows[0], Format.Header.NumberOfRows);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectColumns[] = {0, 2, 1, 0, 1, 1, 2, 3};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.Header.NumberOfColumns);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.Header.NonZeroElements, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 5;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;
        ((f32 *)Out.Elements)[4] = 0.0f;

        CSR_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {7.0f, 6.0f, 0.0f, 14.0f, 65.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 5, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    return 0;
}

static char *
ELL_Tests() 
{
    u32 Error;
    char *ErrorMessage;

    // NOTE(miha): Tests on Matrix.
    {
        mu_scope("Matrix init");
        ell Format = {};
        Error = ELL_ConvertFromMatrixMarket(&Format, &Matrix);
        mu_assert(mu_str("Error converting to Format format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &Matrix);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectColumns[] = {1, 2, 3, 0, 2, 0, 0, 1, 3, 2, 0, 0};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.ElementsPerRow * Format.Header.NumberOfRows);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 9.0f, 3.0f, 4.0f, 0.0f, 5.0f, 6.0f, 8.0f, 7.0f, 0.0f, 0.0f};
        ErrorMessage = TestArray_f32( (f32 *)Format.Elements, &CorrectElements[0], Format.ElementsPerRow * Format.Header.NumberOfRows, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 4;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;

        ELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {44.0f, 15.0f, 49.0f, 21.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 4, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    {
        mu_scope("matrix empty line");
        ell Format = {};
        Error = ELL_ConvertFromMatrixMarket(&Format, &MatrixEmptyLine);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &MatrixEmptyLine);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectColumns[] = {0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 2, 3};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.ElementsPerRow * Format.Header.NumberOfRows);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 5.0f, 0.0f, 6.0f, 7.0f, 8.0f};
        ErrorMessage = TestArray_f32( (f32 *)Format.Elements, &CorrectElements[0], Format.ElementsPerRow * Format.Header.NumberOfRows, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 5;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;
        ((f32 *)Out.Elements)[4] = 0.0f;

        ELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {7.0f, 6.0f, 0.0f, 14.0f, 65.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 5, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    return 0;
}

static char *
SELL_Tests() 
{
    u32 Error;
    char *ErrorMessage;

    // NOTE(miha): Tests on Matrix.
    {
        mu_scope("Matrix init");
        sell Format = {};
        Error = SELL_ConvertFromMatrixMarket(&Format, &Matrix);
        mu_assert(mu_str("Error converting to Format format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &Matrix);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectColumns[] = {1, 2, 3, 0, 2, 0, 0, 1, 3, 2, 0, 0};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.AllocateSize);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 9.0f, 3.0f, 4.0f, 0.0f, 5.0f, 6.0f, 8.0f, 7.0f, 0.0f, 0.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.AllocateSize, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectSliceOffset[] = {0, 6, 12};
        ErrorMessage = TestArray_u32((u32 *)Format.SliceOffset, &CorrectSliceOffset[0], Format.NumberOfSlices + 1);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 4;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;

        SELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {44.0f, 15.0f, 49.0f, 21.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 4, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    {
        mu_scope("matrix empty line");
        sell Format = {};
        Error = SELL_ConvertFromMatrixMarket(&Format, &MatrixEmptyLine);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &MatrixEmptyLine);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements

        u32 CorrectColumns[] = {0, 2, 1, 0, 0, 0, 0, 1, 1, 2, 3, 0, 0, 0};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.AllocateSize);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 3.0f, 0.0f, 0.0f, 0.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 0.0f, 0.0f, 0.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.AllocateSize, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectSliceOffset[] = {0, 4, 8, 14};
        ErrorMessage = TestArray_u32((u32 *)Format.SliceOffset, &CorrectSliceOffset[0], Format.NumberOfSlices + 1);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 5;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;
        ((f32 *)Out.Elements)[4] = 0.0f;

        SELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {7.0f, 6.0f, 0.0f, 14.0f, 65.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 5, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    return 0;
}

static char *
DIA_Tests() 
{
    u32 Error;
    char *ErrorMessage;

    // NOTE(miha): Tests on Matrix.
    {
        mu_scope("Matrix init");
        //void *Elements;
        //i32 *Offsets;
        dia Format = {};
        Error = DIA_ConvertFromMatrixMarket(&Format, &Matrix);
        mu_assert(mu_str("Error converting to Format format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &Matrix);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"), Format.Offsets != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements
        mu_assert(mu_str("Number of diagonals is wrong"), Format.NumberOfDiagonals == 5);
        mu_assert(mu_str("Length of main diagonal is wrong"), Format.MainDiagonalLength == 4);

        i32 CorrectOffsets[] = {-2,-1,1,2,3};
        ErrorMessage = TestArray_i32(Format.Offsets, &CorrectOffsets[0], Format.NumberOfDiagonals);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {5.000000, 0.000000, 0.000000, 0.000000, 3.000000, 6.000000, 7.000000, 0.000000, 1.000000, 4.000000, 8.000000, 0.000000, 2.000000, 0.000000, 0.000000, 0.000000, 9.000000, 0.000000, 0.000000, 0.000000};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.MainDiagonalLength*Format.NumberOfDiagonals, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 4;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;

        DIA_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {44.0f, 15.0f, 49.0f, 21.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 4, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    {
        mu_scope("matrix empty line");
        dia Format = {};
        Error = DIA_ConvertFromMatrixMarket(&Format, &MatrixEmptyLine);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &MatrixEmptyLine);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"), Format.Offsets != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements
        mu_assert(mu_str("Number of diagonals is wrong"), Format.NumberOfDiagonals == 5);
        mu_assert(mu_str("Length of main diagonal is wrong"), Format.MainDiagonalLength == 4);

        i32 CorrectColumns[] = {-3,-2,-1,0,2};
        ErrorMessage = TestArray_i32(Format.Offsets, &CorrectColumns[0], Format.NumberOfDiagonals);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {4.000000, 6.000000, 0.000000, 0.000000, 0.000000, 5.000000, 7.000000, 0.000000, 0.000000, 0.000000, 0.000000, 8.000000, 1.000000, 3.000000, 0.000000, 0.000000, 2.000000, 0.000000, 0.000000, 0.000000};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0], Format.NumberOfDiagonals*Format.MainDiagonalLength, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 5;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;
        ((f32 *)Out.Elements)[4] = 0.0f;

        DIA_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {7.0f, 6.0f, 0.0f, 14.0f, 65.0f};
        ErrorMessage = TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 5, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    return 0;
}

#if 0
static char *
JAD_Tests() 
{
    u32 Error;
    char *ErrorMessage;

    // NOTE(miha): Tests on Matrix.
    {
        mu_scope("Matrix init");
        jad Format = {};
        Error = JAD_ConvertFromMatrixMarket(&Format, &Matrix);
        mu_assert(mu_str("Error converting to Format format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &Matrix);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"), Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"), Format.Elements != NULL); // elements
        mu_assert(mu_str("Row pointers not allocated"), Format.RowPointers != NULL); // columns
        mu_assert(mu_str("Permutations not allocated"), Format.Permutations != NULL); // elements

        u32 CorrectColumns[] = {1, 2, 3, 0, 2, 0, 0, 1, 3, 2, 0, 0};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0], Format.AllocateSize);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 9.0f, 3.0f, 4.0f, 0.0f,
            5.0f, 6.0f, 8.0f, 7.0f, 0.0f, 0.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0],
                                     Format.AllocateSize, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectSliceOffset[] = {0, 6, 12};
        ErrorMessage =
            TestArray_u32((u32 *)Format.SliceOffset, &CorrectSliceOffset[0],
                          Format.NumberOfSlices + 1);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 4;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;

        SELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {44.0f, 15.0f, 49.0f, 21.0f};
        ErrorMessage =
            TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 4, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    {
        mu_scope("matrix empty line");
        sell Format = {};
        Error = SELL_ConvertFromMatrixMarket(&Format, &MatrixEmptyLine);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &MatrixEmptyLine);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"),
                  Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"),
                  Format.Elements != NULL); // elements

        u32 CorrectColumns[] = {0, 2, 1, 0, 0, 0, 0, 1, 1, 2, 3, 0, 0, 0};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0],
                                     Format.AllocateSize);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 3.0f, 0.0f, 0.0f, 0.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f, 0.0f, 0.0f, 0.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0],
                                     Format.AllocateSize, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectSliceOffset[] = {0, 4, 8, 14};
        ErrorMessage =
            TestArray_u32((u32 *)Format.SliceOffset, &CorrectSliceOffset[0],
                          Format.NumberOfSlices + 1);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 5;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;
        ((f32 *)Out.Elements)[4] = 0.0f;

        SELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {7.0f, 6.0f, 0.0f, 14.0f, 65.0f};
        ErrorMessage =
            TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 5, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    return 0;
}
#endif

static char *
BCSR_Tests() 
{
    u32 Error;
    char *ErrorMessage;

    // NOTE(miha): Tests on Matrix.
    {
        mu_scope("Matrix init");
        sell Format = {};
        Error = SELL_ConvertFromMatrixMarket(&Format, &Matrix);
        mu_assert(mu_str("Error converting to Format format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &Matrix);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"),
                  Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"),
                  Format.Elements != NULL); // elements

        u32 CorrectColumns[] = {1, 2, 3, 0, 2, 0, 0, 1, 3, 2, 0, 0};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0],
                                     Format.AllocateSize);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 9.0f, 3.0f, 4.0f, 0.0f,
            5.0f, 6.0f, 8.0f, 7.0f, 0.0f, 0.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0],
                                     Format.AllocateSize, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectSliceOffset[] = {0, 6, 12};
        ErrorMessage =
            TestArray_u32((u32 *)Format.SliceOffset, &CorrectSliceOffset[0],
                          Format.NumberOfSlices + 1);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 4;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;

        SELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {44.0f, 15.0f, 49.0f, 21.0f};
        ErrorMessage =
            TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 4, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    {
        mu_scope("matrix empty line");
        sell Format = {};
        Error = SELL_ConvertFromMatrixMarket(&Format, &MatrixEmptyLine);
        mu_assert(mu_str("Error converting to COO format"), Error == 0);

        ErrorMessage = TestMatrixMarketHeader(&Format.Header, &MatrixEmptyLine);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_assert(mu_str("Columns not allocated"),
                  Format.Columns != NULL); // columns
        mu_assert(mu_str("Elements not allocated"),
                  Format.Elements != NULL); // elements

        u32 CorrectColumns[] = {0, 2, 1, 0, 0, 0, 0, 1, 1, 2, 3, 0, 0, 0};
        ErrorMessage = TestArray_u32((u32 *)Format.Columns, &CorrectColumns[0],
                                     Format.AllocateSize);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        f32 CorrectElements[] = {1.0f, 2.0f, 3.0f, 0.0f, 0.0f, 0.0f, 4.0f,
            5.0f, 6.0f, 7.0f, 8.0f, 0.0f, 0.0f, 0.0f};
        ErrorMessage = TestArray_f32((f32 *)Format.Elements, &CorrectElements[0],
                                     Format.AllocateSize, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        u32 CorrectSliceOffset[] = {0, 4, 8, 14};
        ErrorMessage =
            TestArray_u32((u32 *)Format.SliceOffset, &CorrectSliceOffset[0],
                          Format.NumberOfSlices + 1);
        mu_assert(ErrorMessage, ErrorMessage == 0);

        mu_scope("Matrix vector multiplication");
        vector In = {};
        In.Length = 4;
        In.Field = REAL;
        In.FieldSize = 4;
        In.Elements = malloc(In.Length * In.FieldSize);
        ((f32 *)In.Elements)[0] = 1.0f;
        ((f32 *)In.Elements)[1] = 2.0f;
        ((f32 *)In.Elements)[2] = 3.0f;
        ((f32 *)In.Elements)[3] = 4.0f;

        vector Out = {};
        Out.Length = 5;
        Out.Field = REAL;
        Out.FieldSize = 4;
        Out.Elements = malloc(Out.Length * Out.FieldSize);
        ((f32 *)Out.Elements)[0] = 0.0f;
        ((f32 *)Out.Elements)[1] = 0.0f;
        ((f32 *)Out.Elements)[2] = 0.0f;
        ((f32 *)Out.Elements)[3] = 0.0f;
        ((f32 *)Out.Elements)[4] = 0.0f;

        SELL_SpMV(&Format, &In, &Out);
        f32 CorrectVectorOut[] = {7.0f, 6.0f, 0.0f, 14.0f, 65.0f};
        ErrorMessage =
            TestArray_f32((f32 *)Out.Elements, &CorrectVectorOut[0], 5, EPSILON);
        mu_assert(ErrorMessage, ErrorMessage == 0);
    }

    return 0;
}

static char *Tests() {
    mu_run_test(COO_Tests);
    mu_run_test(CSR_Tests);
    mu_run_test(ELL_Tests);
    mu_run_test(SELL_Tests);
    mu_run_test(DIA_Tests);
    return 0;
}

int 
main() 
{
    // NOTE(miha): Setup matrices for testing (in MatrixMarket format).
    FILE *MatrixFile = fopen("test_data/matrix.mtx", "r");
    MM_ReadFile(MatrixFile, &Matrix);

    FILE *MatrixEmptyLineFile = fopen("test_data/matrix_empty_line.mtx", "r");
    MM_ReadFile(MatrixEmptyLineFile, &MatrixEmptyLine);

    FILE *MatrixBCSRFile = fopen("test_data/matrix_bcsr.mtx", "r");
    MM_ReadFile(MatrixBCSRFile, &MatrixBCSR);

    dia DIA = {};
    DIA_ConvertFromMatrixMarket(&DIA, &MatrixEmptyLine);
    vector In = {};
    In.Length = 4;
    In.Field = REAL;
    In.FieldSize = 4;
    In.Elements = malloc(In.Length * In.FieldSize);
    ((f32 *)In.Elements)[0] = 1.0f;
    ((f32 *)In.Elements)[1] = 2.0f;
    ((f32 *)In.Elements)[2] = 3.0f;
    ((f32 *)In.Elements)[3] = 4.0f;

    vector Out = {};
    Out.Length = 5;
    Out.Field = REAL;
    Out.FieldSize = 4;
    Out.Elements = malloc(Out.Length * Out.FieldSize);
    ((f32 *)Out.Elements)[0] = 0.0f;
    ((f32 *)Out.Elements)[1] = 0.0f;
    ((f32 *)Out.Elements)[2] = 0.0f;
    ((f32 *)Out.Elements)[3] = 0.0f;
    ((f32 *)Out.Elements)[4] = 0.0f;

    for(u32 I = 0; I < DIA.NumberOfDiagonals*DIA.MainDiagonalLength; ++I)
        printf("%f, ", MM_ToArray(DIA.Elements, f32)[I]);
    printf("\n");

    DIA_SpMV(&DIA, &In, &Out);
    PrintVector(&Out);

    /*
    jad JAD = {};
    JAD_ConvertFromMatrixMarket(&JAD, &MatrixEmptyLine);
    vector In = {};
    In.Length = 4;
    In.Field = REAL;
    In.FieldSize = 4;
    In.Elements = malloc(In.Length * In.FieldSize);
    ((f32 *)In.Elements)[0] = 1.0f;
    ((f32 *)In.Elements)[1] = 2.0f;
    ((f32 *)In.Elements)[2] = 3.0f;
    ((f32 *)In.Elements)[3] = 4.0f;

    vector Out = {};
    Out.Length = 5;
    Out.Field = REAL;
    Out.FieldSize = 4;
    Out.Elements = malloc(Out.Length * Out.FieldSize);
    ((f32 *)Out.Elements)[0] = 0.0f;
    ((f32 *)Out.Elements)[1] = 0.0f;
    ((f32 *)Out.Elements)[2] = 0.0f;
    ((f32 *)Out.Elements)[3] = 0.0f;
    ((f32 *)Out.Elements)[4] = 0.0f;
    JAD_SpMV(&JAD, &In, &Out);
    PrintVector(&Out);
    */

    // TODO(miha): Create tests for BCSR format.
#if 0
    bcsr BCSR = {};
    BCSR_ConvertFromMatrixMarket(&BCSR, &Matrix);
    vector In = {};
    In.Length = 4;
    In.Field = REAL;
    In.FieldSize = 4;
    In.Elements = malloc(In.Length * In.FieldSize);
    ((f32 *)In.Elements)[0] = 1.0f;
    ((f32 *)In.Elements)[1] = 2.0f;
    ((f32 *)In.Elements)[2] = 3.0f;
    ((f32 *)In.Elements)[3] = 4.0f;
    /*
    ((f32 *)In.Elements)[4] = 1.0f;
    ((f32 *)In.Elements)[5] = 2.0f;
    ((f32 *)In.Elements)[6] = 3.0f;
    ((f32 *)In.Elements)[7] = 4.0f;
    */

    vector Out = {};
    Out.Length = 4;
    Out.Field = REAL;
    Out.FieldSize = 4;
    Out.Elements = malloc(Out.Length * Out.FieldSize);
    ((f32 *)Out.Elements)[0] = 0.0f;
    ((f32 *)Out.Elements)[1] = 0.0f;
    ((f32 *)Out.Elements)[2] = 0.0f;
    ((f32 *)Out.Elements)[3] = 0.0f;
    /*
    ((f32 *)Out.Elements)[4] = 0.0f;
    ((f32 *)Out.Elements)[5] = 0.0f;
    ((f32 *)Out.Elements)[6] = 0.0f;
    ((f32 *)Out.Elements)[7] = 0.0f;
    */
    //((f32 *)Out.Elements)[4] = 0.0f;
    BCSR_SpMV(&BCSR, &In, &Out);
    PrintVector(&Out);
#endif

    // TODO(miha): Create tests for DIA format.
#if 0
    dia DIA = {};
    DIA_ConvertFromMatrixMarket(&DIA, &Matrix);
    vector In = {};
    In.Length = 4;
    In.Field = REAL;
    In.FieldSize = 4;
    In.Elements = malloc(In.Length * In.FieldSize);
    ((f32 *)In.Elements)[0] = 1.0f;
    ((f32 *)In.Elements)[1] = 2.0f;
    ((f32 *)In.Elements)[2] = 3.0f;
    ((f32 *)In.Elements)[3] = 4.0f;

    vector Out = {};
    Out.Length = 4;
    Out.Field = REAL;
    Out.FieldSize = 4;
    Out.Elements = malloc(Out.Length * Out.FieldSize);
    ((f32 *)Out.Elements)[0] = 0.0f;
    ((f32 *)Out.Elements)[1] = 0.0f;
    ((f32 *)Out.Elements)[2] = 0.0f;
    ((f32 *)Out.Elements)[3] = 0.0f;
    //((f32 *)Out.Elements)[4] = 0.0f;
    DIA_SpMV(&DIA, &In, &Out);
    PrintVector(&Out);
#endif

    char *Result = Tests();
    if (Result != 0) {
        printf("%s\n", Result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return Result != 0;
}

#endif // TESTS_H
