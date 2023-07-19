/*********************************************
* File - tests.h
* Author - Miha
* Created - 12 jul 2023
* Description - 
* *******************************************/
#if !defined(TESTS_H)
#define TESTS_H

// NOTE(miha): Copied from: https://jera.com/techinfo/jtns/jtn002.
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; if (message) return message; } while (0)
extern int tests_run;
int tests_run = 0;

#include "converter_main.h"
#include "converter_read_matrix_market.h"

static matrix_market Matrix = {};
static matrix_market MatrixEmptyLine = {};

static char *
COO_Tests()
{
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

    FILE *MatrixEmptyLineFile = fopen("test_date/matrix_empty_line.mtx", "r");
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
