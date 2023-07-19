/*********************************************
* File - converter_main.cpp
* Author - Miha
* Created - 26 jun 2023
* Description - 
* *******************************************/
#include "converter_main.h"
#include "converter_read_matrix_market.h"
// TODO(miha): Combine together converter and blas header files.
#include "converter_coo.h"
#include "converter_csr.h"
#include "converter_ell.h"
#include "converter_sell.h"
#include "blas.h"
#include "blas_coo.h"
#include "blas_csr.h"
#include "blas_ell.h"

int
main()
{
    // FILE *File = fopen("494_bus.mtx", "r");
    FILE *File = fopen("test_matrix.mtx", "r");
    matrix_market MatrixMarket = {};
    MM_ReadFile(File, &MatrixMarket);
    MM_PrintHeader(&MatrixMarket.Header);

    coo COO = {};
    COO_ConvertFromMatrixMarket(&COO, &MatrixMarket);

    csr CSR = {};
    CSR_ConvertFromMatrixMarket(&CSR, &MatrixMarket);

    ell ELL = {};
    ELL_ConvertFromMatrixMarket(&ELL, &MatrixMarket);

    //sell SELL = {};
    //SELL_ConvertFromMatrixMarket(&SELL, &MatrixMarket);
    
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

    PrintVector(&In);
    PrintVector(&Out);
    printf("--------\n");

    //COO_SpMV(&COO, &In, &Out);
    CSR_SpMV(&CSR, &In, &Out);
    //ELL_SpMV(&ELL, &In, &Out);

    PrintVector(&Out);

    /*
    PrintVector(&Out);
    vector RVector = RandomVector(4, 4);
    PrintVector(&RVector);
    vector IVector = IdentityVector(4, 4);
    PrintVector(&IVector);

    matrix M = IdentityMatrix(4, 4);
    PrintMatrix(&M);
    
    matrix R = RandomMatrix(4, 4, 4, 0);
    PrintMatrix(&R);

    MatrixVectorMultiply(&R, &In, &Out);
    PrintVector(&Out);

    matrix S = RandomSparseMatrix(4, 4, 0.5f);
    PrintMatrix(&S);
    */

    fclose(File);
}
