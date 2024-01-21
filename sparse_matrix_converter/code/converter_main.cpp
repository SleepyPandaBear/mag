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
#include "blas_bcoo.h"
#include "blas_coo.h"
#include "blas_csr.h"
#include "blas_ell.h"
#include "converter_block_coo.h"
#include "converter_coo.h"
#include "converter_csr.h"
#include "converter_dia.h"
#include "converter_ell.h"
#include "converter_sell.h"

int main() {
  // FILE *File = fopen("494_bus.mtx", "r");
  FILE *File = fopen("dwt_512.mtx", "r");
  matrix_market MatrixMarket = {};
  MM_ReadFile(File, &MatrixMarket);
  MM_PrintHeader(&MatrixMarket.Header);
  MM_InsertSymetricValues(&MatrixMarket);

  // TODO:
  //   - MatrixMarket convert formats
  //   - Converters output format
  //   - better vectors/matrices, add type
  //   -
  //   -

  bcoo BCOO = {};
  BCOO.BlockWidth = 8;
  BCOO.BlockHeight = 8;

#define N 512

  coo COO = {};
  COO_ConvertFromMatrixMarket(&COO, &MatrixMarket);

  vector V_In = IdentityVector(N, sizeof(u32));

  vector V_OutBCOO = ZeroVector(N, sizeof(u32));
  BCOO_ConvertFromMatrixMarket(&BCOO, &MatrixMarket);
  // BCOO_Print(&BCOO);
  BCOO_SpMV(&BCOO, &V_In, &V_OutBCOO);
  PrintVector(&V_OutBCOO);

  vector V_OutCOO = ZeroVector(N, sizeof(u32));
  COO_SpMV(&COO, &V_In, &V_OutCOO);
  PrintVector(&V_OutCOO);

  fclose(File);
}
