/*********************************************
 * File - converter_main.cpp
 * Author - Miha
 * Created - 26 jun 2023
 * Description -
 * *******************************************/
#include "converter_main.h"
#include "converter_read_matrix_market.h"
// TODO(miha): Combine together converter and blas header files.
#include "blas.h"
#include "blas_coo.h"
#include "blas_csr.h"
#include "blas_ell.h"
#include "converter_coo.h"
#include "converter_csr.h"
#include "converter_dia.h"
#include "converter_ell.h"
#include "converter_sell.h"
#include "converter_block_coo.h"

int main() 
{
    // FILE *File = fopen("494_bus.mtx", "r");
    FILE *File = fopen("my_dwt_512.mtx", "r");
    matrix_market MatrixMarket = {};
    MM_ReadFile(File, &MatrixMarket);
    MM_PrintHeader(&MatrixMarket.Header);
    MM_InsertSymetricValues(&MatrixMarket);

    bcoo BCOO = {};
    BCOO.BlockWidth = 4;
    BCOO.BlockHeight = 4;

    BCOO_ConvertFromMatrixMarket(&BCOO, &MatrixMarket);
    BCOO_Print(&BCOO);

    fclose(File);
}
