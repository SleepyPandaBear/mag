#ifndef _KERNEL_H
#define _KERNEL_H

#define BLOCK_SIZE (8)

struct block {
    int Row;
    int Column;
    int Elements[BLOCK_SIZE*BLOCK_SIZE];
};

typedef int dtype;

void block_spmv(block Block, int *In, int *Out);
void kernel(block *Blocks, int N, int *In, int *Out);

#endif // _KERNEL_H
