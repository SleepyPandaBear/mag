/*********************************************
 * File - converter_read_matrix_market.h
 * Author - Miha
 * Created - 29 jun 2023
 * Description -
 * *******************************************/
#if !defined(CONVERTER_READ_MATRIX_MARKET_H)
#define CONVERTER_READ_MATRIX_MARKET_H

#include "converter_main.h"
#include "stdio.h"
#include "stdlib.h"

// NOTE(miha): Got this magic values from:
// https://math.nist.gov/MatrixMarket/mmio/c/mmio.h
#define LINE_LENGTH  (1025)
#define TOKEN_LENGTH (64)

#define MM_ToArray(Pointer, Type) ((Type *)Pointer)

enum matrix_market_errors
{
    FGETS_ERR,
    HEADER_LINE_ERR,
    NOT_IMPLEMENTED_ERR,
    UNKNOWN_VALUE_ERR,
    SSCANF_ERR,
    MALLOC_ERR,
};

enum matrix_market_object
{
    MATRIX,
    VECTOR,
};

enum matrix_market_format
{
    COORDINATE,
    ARRAY,
};

enum matrix_market_field
{
    REAL,
    DOUBLE,
    COMPLEX,
    INTEGER,
    PATTERN,
};

enum matrix_market_symmetry
{
    GENERAL,
    SYMMETRIC,
    SKEW_SYMMETRIC,
    HERMITIAN,
};

// NOTE(miha): We can see this TODO
struct matrix_market_header
{
    // NOTE(miha): Header line information saved in strings.
    char BannerString[TOKEN_LENGTH];
    char ObjectString[TOKEN_LENGTH];
    char FormatString[TOKEN_LENGTH];
    char FieldString[TOKEN_LENGTH];
    char SymmetryString[TOKEN_LENGTH];

    matrix_market_object Object;
    matrix_market_format Format;
    matrix_market_field Field;
    matrix_market_symmetry Symmetry;

    u32 FieldSize;

    u32 NumberOfRows;
    u32 NumberOfColumns;
    u32 NonZeroElements;
};

struct matrix_market
{
    matrix_market_header Header;

    u32 *Rows;
    u32 *Columns;
    void *Elements;
};

internal inline u32
MM_ReadLine(FILE *File, char *Line, u32 Length)
{
    if(fgets(Line, Length, File) == NULL)
    {
        return FGETS_ERR;
    }

    return 0;
}

internal inline u32
MM_ReadHeaderLine(FILE *File,
    matrix_market_header *Header)
{
    char Line[LINE_LENGTH];

    // NOTE(miha): Try to read first file's line.
    u32 Error = MM_ReadLine(File, &Line[0], LINE_LENGTH);
    if(Error)
        return Error;

    if(sscanf(Line, "%s %s %s %s %s", Header->BannerString, Header->ObjectString,
           Header->FormatString, Header->FieldString,
           Header->SymmetryString) != 5)
    {
        return HEADER_LINE_ERR;
    }

    ToLower(&Header->ObjectString[0]);
    ToLower(&Header->FormatString[0]);
    ToLower(&Header->FieldString[0]);
    ToLower(&Header->SymmetryString[0]);

    if(CompareString(&Header->ObjectString[0], "matrix"))
    {
        Header->Object = MATRIX;
    }
    else if(CompareString(&Header->ObjectString[0], "vector"))
    {
        Header->Object = VECTOR;
    }
    else
    {
        return UNKNOWN_VALUE_ERR;
    }

    if(CompareString(&Header->FormatString[0], "coordinate"))
    {
        Header->Format = COORDINATE;
    }
    else if(CompareString(&Header->FormatString[0], "array"))
    {
        Header->Format = ARRAY;
    }
    else
    {
        return UNKNOWN_VALUE_ERR;
    }

    if(CompareString(&Header->FieldString[0], "real"))
    {
        Header->Field = REAL;
        Header->FieldSize = 4;
    }
    else if(CompareString(&Header->FieldString[0], "double"))
    {
        Header->Field = DOUBLE;
        Header->FieldSize = 8;
    }
    else if(CompareString(&Header->FieldString[0], "integer"))
    {
        Header->Field = INTEGER;
        Header->FieldSize = 4;
    }
    else if(CompareString(&Header->FieldString[0], "complex"))
    {
        Header->Field = COMPLEX;
        Header->FieldSize = 16; // TODO(miha): Is this 16 bytes wide?
    }
    else if(CompareString(&Header->FieldString[0], "pattern"))
    {
        Header->Field = PATTERN;
        Header->FieldSize = 4;
    }
    else
    {
        return UNKNOWN_VALUE_ERR;
    }

    if(CompareString(&Header->SymmetryString[0], "general"))
    {
        Header->Symmetry = GENERAL;
    }
    else if(CompareString(&Header->SymmetryString[0], "symmetric"))
    {
        Header->Symmetry = SYMMETRIC;
    }
    else if(CompareString(&Header->SymmetryString[0], "skew-symmetric"))
    {
        Header->Symmetry = SKEW_SYMMETRIC;
    }
    else if(CompareString(&Header->SymmetryString[0], "hermitian"))
    {
        Header->Symmetry = HERMITIAN;
    }
    else
    {
        return UNKNOWN_VALUE_ERR;
    }

    // TODO(miha): matrix can only be hermitian if it has complex numbers and
    // so on.. handle this cases.

    return 0;
}

internal inline void
MM_PrintHeader(matrix_market_header *Header)
{
    printf("%s %s %s %s %s\n", Header->BannerString, Header->ObjectString,
        Header->FormatString, Header->FieldString, Header->SymmetryString);
}

// NOTE(miha): Here we read an element from the MM format.
internal inline void *
MM_ReadElement()
{
    return NULL;
}

internal inline void
MM_WriteElement(void *Element)
{
}

// NOTE(miha): Here we read an element from file using sscanf function.
// CARE(miha): This function have side effects.
internal inline u32
MM_ParseLine(matrix_market *MatrixMarket, char *Line, u32 Index)
{
    if(MatrixMarket->Header.Field == REAL)
    {
        if(sscanf(Line, "%u %u %f", &MatrixMarket->Rows[Index],
               &MatrixMarket->Columns[Index],
               &((f32 *)MatrixMarket->Elements)[Index]) != 3)
        {
            return SSCANF_ERR;
        }
    }
    else if(MatrixMarket->Header.Field == DOUBLE)
    {
        if(sscanf(Line, "%u %u %lf", &MatrixMarket->Rows[Index],
               &MatrixMarket->Columns[Index],
               &((f64 *)MatrixMarket->Elements)[Index]) != 3)
        {
            return SSCANF_ERR;
        }
    }
    else if(MatrixMarket->Header.Field == INTEGER)
    {
        if(sscanf(Line, "%u %u %d", &MatrixMarket->Rows[Index],
               &MatrixMarket->Columns[Index],
               &((i32 *)MatrixMarket->Elements)[Index]) != 3)
        {
            return SSCANF_ERR;
        }
    }
    else if(MatrixMarket->Header.Field == COMPLEX)
    {
        // TODO(miha): See how complex numbers are implemented.
        if(sscanf(Line, "%u %u %f", &MatrixMarket->Rows[Index],
               &MatrixMarket->Columns[Index],
               &((f32 *)MatrixMarket->Elements)[Index]) != 3)
        {
            return SSCANF_ERR;
        }
    }
    else if(MatrixMarket->Header.Field == PATTERN)
    {
        if(sscanf(Line, "%u %u", &MatrixMarket->Rows[Index],
               &MatrixMarket->Columns[Index]) != 2)
        {
            return SSCANF_ERR;
        }
        ((i32 *)MatrixMarket->Elements)[Index] = 1;
    }
    else
    {
        return UNKNOWN_VALUE_ERR;
    }

    // NOTE(miha): Convert to 0 index.
    MatrixMarket->Rows[Index]--;
    MatrixMarket->Columns[Index]--;

    return 0;
}

internal inline u32
MM_ReadFile(FILE *File, matrix_market *MatrixMarket)
{
    u32 Error;
    char Line[LINE_LENGTH];

    Error = MM_ReadHeaderLine(File, &MatrixMarket->Header);
    if(Error)
    {
        return Error;
    }

    // NOTE(miha): Skip comment lines.
    Error = MM_ReadLine(File, &Line[0], LINE_LENGTH);
    while(Line[0] == '%' || Line[0] == '#')
    {
        Error = MM_ReadLine(File, &Line[0], LINE_LENGTH);
    }
    if(Error)
    {
        return Error;
    }

    // NOTE(miha): Read number of rows, number of columns and number of non
    // zero elements.
    if(sscanf(Line, "%u %u %u", &MatrixMarket->Header.NumberOfRows,
           &MatrixMarket->Header.NumberOfColumns,
           &MatrixMarket->Header.NonZeroElements) != 3)
    {
        return SSCANF_ERR;
    }

    // NOTE(miha): Allocate memory for arrays.
    MatrixMarket->Rows =
        (u32 *)malloc(MatrixMarket->Header.NonZeroElements * sizeof(u32));
    if(MatrixMarket->Rows == NULL)
    {
        return MALLOC_ERR;
    }
    MatrixMarket->Columns =
        (u32 *)malloc(MatrixMarket->Header.NonZeroElements * sizeof(u32));
    if(MatrixMarket->Columns == NULL)
    {
        return MALLOC_ERR;
    }
    MatrixMarket->Elements = malloc(MatrixMarket->Header.NonZeroElements *
        MatrixMarket->Header.FieldSize);
    if(MatrixMarket->Elements == NULL)
    {
        return MALLOC_ERR;
    }

    // NOTE(miha): Read elements.
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        Error = MM_ReadLine(File, &Line[0], LINE_LENGTH);
        if(Error)
            return Error;

        Error = MM_ParseLine(MatrixMarket, &Line[0], I);
        if(Error)
            return Error;
    }

    return 0;
}

// NOTE(miha): Some matrices have symetric MM format, this means only elements
// on or above diagonal are inputed. This function insert "missing" elements
// below the diagonal line.
internal inline u32
MM_InsertSymetricValues(matrix_market *MatrixMarket)
{
    u32 Error;
    char Line[LINE_LENGTH];

    // NOTE(miha): Count how many elements are on and above the diagonal.
    u32 ElementsAboveDiagonal = 0;
    u32 ElementsOnDiagonal = 0;
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        if(MM_ToArray(MatrixMarket->Rows, u32)[I] ==
            MM_ToArray(MatrixMarket->Columns, u32)[I])
        {
            ElementsOnDiagonal++;
        }
        else
        {
            ElementsAboveDiagonal++;
        }
    }

    u32 NumberOfElements = ElementsAboveDiagonal * 2 + ElementsOnDiagonal;

    // NOTE(miha): Allocate memory for arrays.
    u32 *NewRows = (u32 *)malloc(NumberOfElements * sizeof(u32));
    if(NewRows == NULL)
    {
        return MALLOC_ERR;
    }
    u32 *NewColumns = (u32 *)malloc(NumberOfElements * sizeof(u32));
    if(NewColumns == NULL)
    {
        return MALLOC_ERR;
    }
    void *NewElements = malloc(NumberOfElements * MatrixMarket->Header.FieldSize);
    if(NewElements == NULL)
    {
        return MALLOC_ERR;
    }

    // NOTE(miha): Read elements.
    u32 NewIndex = 0;
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        if(MatrixMarket->Header.Field == REAL)
        {
        }
        else if(MatrixMarket->Header.Field == DOUBLE)
        {
        }
        else if(MatrixMarket->Header.Field == INTEGER)
        {
        }
        else if(MatrixMarket->Header.Field == COMPLEX)
        {
        }
        else if(MatrixMarket->Header.Field == PATTERN)
        {
            if(MM_ToArray(MatrixMarket->Rows, u32)[I] ==
                MM_ToArray(MatrixMarket->Columns, u32)[I])
            {
                // NOTE(miha): Diagonal elements.

                MM_ToArray(NewRows, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Rows, u32)[I];
                MM_ToArray(NewColumns, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Columns, u32)[I];
                MM_ToArray(NewElements, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Elements, u32)[I];
                NewIndex++;
            }
            else
            {
                // NOTE(miha): Off-diagonal elements.

                MM_ToArray(NewRows, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Rows, u32)[I];
                MM_ToArray(NewColumns, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Columns, u32)[I];
                MM_ToArray(NewElements, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Elements, u32)[I];
                NewIndex++;

                // NOTE(miha): Put elements also below the main diagonal.
                MM_ToArray(NewColumns, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Rows, u32)[I];
                MM_ToArray(NewRows, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Columns, u32)[I];
                MM_ToArray(NewElements, u32)[NewIndex] =
                    MM_ToArray(MatrixMarket->Elements, u32)[I];
                NewIndex++;
            }
        }
        else
        {
            return UNKNOWN_VALUE_ERR;
        }
    }

    /* MatrixMarket->Header.NumberOfRows = NumberOfElements; */
    /* MatrixMarket->Header.NumberOfColumns = NumberOfElements; */
    MatrixMarket->Header.NonZeroElements = NumberOfElements;

    free(MatrixMarket->Rows);
    free(MatrixMarket->Columns);
    free(MatrixMarket->Elements);
    MatrixMarket->Rows = NewRows;
    MatrixMarket->Columns = NewColumns;
    MatrixMarket->Elements = NewElements;

    return 0;
}

/*
u32
MM_ReadFileFloat(FILE *File, matrix_market *MatrixMarket)
{
    u32 Error = 0;
    for(u32 I = 0; I < MatrixMarket->Header.NonZeroElements; ++I)
    {
        Error = MM_ReadLine(File, &Line[0], LINE_LENGTH);
        if(Error)
        {
            return Error;
        }

        if(MatrixMarket->Header.Field == REAL)
        {
            if(sscanf(Line, "%u %u %f", &((u32 *)MatrixMarket->Rows)[I], &((u32
*)MatrixMarket->Columns)[I],
                      &((f32 *)MatrixMarket->Elements)[I]) != 3)
            {
                return SSCANF_ERR;
            }
            ((u32 *)MatrixMarket->Rows)[I]--;
            ((u32 *)MatrixMarket->Columns)[I]--;
        }
    }
    return Error;
}
*/

// TODO(miha): Implement this
u32
MM_ReadFileTyped(FILE *File, matrix_market *MatrixMarket)
{
    u32 Error;
    char Line[LINE_LENGTH];

    Error = MM_ReadHeaderLine(File, &MatrixMarket->Header);
    if(Error)
    {
        return Error;
    }

    // NOTE(miha): Skip comment lines.
    Error = MM_ReadLine(File, &Line[0], LINE_LENGTH);
    while(Line[0] == '%' || Line[0] == '#')
    {
        Error = MM_ReadLine(File, &Line[0], LINE_LENGTH);
    }
    if(Error)
    {
        return Error;
    }

    // NOTE(miha): Read number of rows, number of columns and number of non
    // zero elements.
    if(sscanf(Line, "%u %u %u", &MatrixMarket->Header.NumberOfRows,
           &MatrixMarket->Header.NumberOfColumns,
           &MatrixMarket->Header.NonZeroElements) != 3)
    {
        return SSCANF_ERR;
    }

    // NOTE(miha): Allocate memory for arrays.
    MatrixMarket->Rows =
        (u32 *)malloc(MatrixMarket->Header.NonZeroElements * sizeof(u32));
    if(MatrixMarket->Rows == NULL)
    {
        return MALLOC_ERR;
    }
    MatrixMarket->Columns =
        (u32 *)malloc(MatrixMarket->Header.NonZeroElements * sizeof(u32));
    if(MatrixMarket->Columns == NULL)
    {
        return MALLOC_ERR;
    }
    MatrixMarket->Elements = malloc(MatrixMarket->Header.NonZeroElements *
        MatrixMarket->Header.FieldSize);
    if(MatrixMarket->Elements == NULL)
    {
        return MALLOC_ERR;
    }

    if(MatrixMarket->Header.Field == REAL)
    {
        // return MM_ReadFileFloat(FILE *File, matrix_market *MatrixMarket);
    }
    else if(MatrixMarket->Header.Field == DOUBLE)
    {
    }
    else if(MatrixMarket->Header.Field == INTEGER)
    {
    }
    else if(MatrixMarket->Header.Field == COMPLEX)
    {
    }
    else if(MatrixMarket->Header.Field == PATTERN)
    {
    }
    else
    {
        return UNKNOWN_VALUE_ERR;
    }

    return 0;
}

#endif // CONVERTER_READ_MATRIX_MARKET_H
