#ifndef _KERNEL_H
#define _KERNEL_H

#include <hls_vector.h>

typedef hls::vector<int, 8> vtype;

#define BLOCK_SIZE (32)

struct block {
    int Row;
    int Column;
    int Elements[BLOCK_SIZE*BLOCK_SIZE];
};

typedef int dtype;

void kernel_block_spmv(int *Elements, int *In, int *Out);

#endif // _KERNEL_H
