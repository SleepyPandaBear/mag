/*********************************************
* File - blas.h
* Author - Miha
* Created - 03 jul 2023
* Description - 
* *******************************************/
#if !defined(BLAS_H)
#define BLAS_H

#include "stdlib.h"
#include "time.h"

#include "converter_read_matrix_market.h"

struct vector
{
    u32 Length;
    matrix_market_field Field;
    u32 FieldSize;
    void *Elements;
};

vector
IdentityVector(u32 N, u32 FieldSize)
{
    vector Result = {};
    Result.Elements = malloc(N*FieldSize);
    Result.Length = N;
    Result.FieldSize = FieldSize;

    for(u32 I = 0; I < N; ++I)
    {
        ((f32 *)Result.Elements)[I] = 1.0f;
    }

    return Result;
}

vector
RandomVector(u32 N, u32 FieldSize, u32 Seed)
{
    srand(Seed);

    vector Result = {};
    Result.Elements = malloc(N*FieldSize);
    Result.Length = N;
    Result.FieldSize = FieldSize;

    for(u32 I = 0; I < N; ++I)
    {
        ((f32 *)Result.Elements)[I] = (f32)(rand()) / ((f32)(RAND_MAX));
    }

    return Result;
}

vector
RandomVector(u32 N, u32 FieldSize)
{
    return RandomVector(N, FieldSize, time(NULL));
}

void
PrintVector(vector *Vector)
{
    for(u32 I = 0; I < Vector->Length; ++I)
    {
        printf("%f, ", MM_ToArray(Vector->Elements, f32)[I]);
    }
    printf("\n");
}

// NOTE(miha): Matrix stores all elemeents, row major.
struct matrix
{
    u32 Rows;
    u32 Columns;
    u32 FieldSize;
    matrix_market_field Field;
    void *Elements;
};

matrix
IdentityMatrix(u32 Rows, u32 Columns, u32 FieldSize)
{
    matrix Result = {};
    Result.Elements = malloc(Rows*Columns*FieldSize);
    Result.Rows = Rows;
    Result.Columns = Columns;
    Result.FieldSize = FieldSize;

    for(u32 R = 0; R < Rows; ++R)
    {
        for(u32 C = 0; C < Columns; ++C)
        {
            if(R == C)
            {
                MM_ToArray(Result.Elements, f32)[R*Columns + C] = 1.0;
            }
            else
            {
                MM_ToArray(Result.Elements, f32)[R*Columns + C] = 0.0;
            }
        }
    }

    return Result;
}

matrix
IdentityMatrix(u32 N, u32 FieldSize)
{
    return IdentityMatrix(N, N, FieldSize);
}

matrix
RandomMatrix(u32 Rows, u32 Columns, u32 FieldSize, u32 Seed)
{
    srand(Seed);

    matrix Result = {};
    Result.Elements = malloc(Rows*Columns*FieldSize);
    Result.Rows = Rows;
    Result.Columns = Columns;
    Result.FieldSize = FieldSize;

    for(u32 R = 0; R < Rows; ++R)
    {
        for(u32 C = 0; C < Columns; ++C)
        {
            MM_ToArray(Result.Elements, f32)[R*Columns + C] = (f32)(rand()) / ((f32)(RAND_MAX));
        }
    }

    return Result;
}

matrix
RandomMatrix(u32 Rows, u32 Columns, u32 FieldSize)
{
    return RandomMatrix(Rows, Columns, FieldSize, time(NULL));
}

matrix
RandomMatrix(u32 N, u32 FieldSize)
{
    return RandomMatrix(N, N, FieldSize, time(NULL));
}

matrix
RandomSparseMatrix(u32 Rows, u32 Columns, u32 FieldSize, u32 Seed, f32 Probability)
{
    matrix Result = {};
    Result.Elements = malloc(Rows*Columns*FieldSize);
    Result.Rows = Rows;
    Result.Columns = Columns;
    Result.FieldSize = FieldSize;

    for(u32 R = 0; R < Rows; ++R)
    {
        for(u32 C = 0; C < Columns; ++C)
        {
            if(Probability < (f32)(rand()) / ((f32)(RAND_MAX)))
                MM_ToArray(Result.Elements, f32)[R*Columns + C] = (f32)(rand()) / ((f32)(RAND_MAX));
            else
                MM_ToArray(Result.Elements, f32)[R*Columns + C] = 0.0f;
        }
    }

    return Result;
}

matrix
RandomSparseMatrix(u32 Rows, u32 Columns, u32 FieldSize, f32 Probability)
{
    return RandomSparseMatrix(Rows, Columns, FieldSize, time(NULL), Probability);
}

matrix
RandomSparseMatrix(u32 N, u32 FieldSize, f32 Probability)
{
    return RandomSparseMatrix(N, N, FieldSize, time(NULL), Probability);
}

void
PrintMatrix(matrix *Matrix)
{
    for(u32 R = 0; R < Matrix->Rows; ++R)
    {
        for(u32 C = 0; C < Matrix->Columns; ++C)
        {
            printf("%2.2f ", MM_ToArray(Matrix->Elements, f32)[R*Matrix->Columns + C]);
        }
        printf("\n");
    }
    printf("\n");
}

void
MatrixVectorMultiply(matrix *Matrix, vector *In, vector *Out)
{
    for(u32 R = 0; R < Matrix->Rows; ++R)
    {
        f32 Sum = 0.0f;
        for(u32 C = 0; C < Matrix->Columns; ++C)
        {
            Sum += MM_ToArray(In->Elements, f32)[C] * MM_ToArray(Matrix->Elements, f32)[R*Matrix->Columns + C];
        }
        MM_ToArray(Out->Elements, f32)[R] = Sum;
    }
}

// TODO(miha):
void
MatrixMarketToMatrix()
{
}

void
MatrixToMatrixMarket()
{
}

void
MatrixMarketToVector()
{
}

void
VectorToMatrixMarket()
{
}

#endif // BLAS_H
