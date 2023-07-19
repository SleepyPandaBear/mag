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
// TODO(miha): Create mu_scope that would print current name of the funciton + scope name.
#define mu_scope(Name) printf("Running tests %s in function %s\n", Name, __func__)
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; if (message) return message; } while (0)
extern int tests_run;
int tests_run = 0;

#include "stdio.h"
#include "math.h"
#include "converter_main.h"
#include "converter_read_matrix_market.h"
#include "converter_coo.h"
#include "converter_csr.h"
#include "converter_ell.h"
#include "converter_sell.h"
#include "blas.h"
#include "blas_coo.h"
#include "blas_csr.h"
#include "blas_ell.h"

#define EPSILON 0.05f

static matrix_market Matrix = {};
static matrix_market MatrixEmptyLine = {};

static char *
TestArray_f32(f32 *A, f32 *B, u32 Length, f32 Epsilon)
{
    for(u32 I = 0; I < Length; ++I)
    {
        if(fabs(A[I]-B[I]) > Epsilon)
        {
            char Buffer[256];
            sprintf(Buffer, "%f != %f at index %d\n", A[I], B[I], I);
            printf("%s", Buffer);
            return mu_str("error happended");
        }
    }

    return 0;
}

static char *
TestArray_u32(u32 *A, u32 *B, u32 Length)
{
    for(u32 I = 0; I < Length; ++I)
    {
        if(A[I] != B[I])
        {
            char Buffer[256];
            sprintf(Buffer, "%d != %d at index %d\n", A[I], B[I], I);
            printf("%s", Buffer);
            return mu_str("error happended");
        }
    }

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
    coo COO = {};
    Error = COO_ConvertFromMatrixMarket(&COO, &Matrix);
    mu_assert(mu_str("Error converting to COO format"), Error == 0);
    // TODO(miha): Write a function that test headers of format and matrix
    // market. All formats should have matrix_market_format.
    mu_assert(mu_str("Object differs"), COO.Object == Matrix.Header.Object); // object
    mu_assert(mu_str("Format differs"), COO.Format == Matrix.Header.Format); // format
    mu_assert(mu_str("Field differs"), COO.Field == Matrix.Header.Field); // field 
    mu_assert(mu_str("Symmetry differs"), COO.Symmetry == Matrix.Header.Symmetry); // symmetry
    mu_assert(mu_str("Fieldsize differs"), COO.FieldSize == Matrix.Header.FieldSize); // fieldsize
    mu_assert(mu_str("Number of rows differs"), COO.NumberOfRows == Matrix.Header.NumberOfRows); // number of rows
    mu_assert(mu_str("Number of columns differs"), COO.NumberOfColumns == Matrix.Header.NumberOfColumns); // number of columns
    mu_assert(mu_str("Number of non zero elements differs"), COO.NonZeroElements == Matrix.Header.NonZeroElements); // number of non zero elements
    mu_assert(mu_str("Rows not allocated"), COO.Rows != NULL); // rows 
    mu_assert(mu_str("Columns not allocated"), COO.Columns != NULL); // columns
    mu_assert(mu_str("Elements not allocated"), COO.Elements != NULL); // elements
 
    u32 CorrectRows[] = {0, 0, 0, 1, 1, 2, 2, 2, 3};
    ErrorMessage = TestArray_u32((u32 *)COO.Rows, &CorrectRows[0], COO.NonZeroElements);
    mu_assert(ErrorMessage, ErrorMessage == 0);

    u32 CorrectColumns[] = {1,2,3,0,2,0,1,3,2};
    ErrorMessage = TestArray_u32((u32 *)COO.Columns, &CorrectColumns[0], COO.NumberOfColumns);
    mu_assert(ErrorMessage, ErrorMessage == 0);

    f32 CorrectElements[] = {1.0f,2.0f,9.0f,3.0f,4.0f,5.0f,6.0f,8.0f,7.0f};
    ErrorMessage = TestArray_f32((f32 *)COO.Elements, &CorrectElements[0], COO.NonZeroElements, EPSILON);
    mu_assert(ErrorMessage, ErrorMessage == 0);

    mu_scope("Matrix vector multiplication");
    }

    // NOTE(miha): Tests on MatrixEmptyLine.
    coo COO_MatrixEmptyLine = {};
    COO_ConvertFromMatrixMarket(&COO_MatrixEmptyLine, &MatrixEmptyLine);
    return 0;
}

static char *
Tests()
{
    mu_run_test(COO_Tests);
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

    char * Result = Tests();
    if(Result != 0)
    {
        printf("%s\n", Result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return Result != 0;
}

#endif // TESTS_H
