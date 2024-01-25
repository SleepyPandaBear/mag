#include "kernel.h"
#include "stdio.h"

void
block_spmv(block Block, int In[], int Out[])
{
    for(unsigned int ElementIndex = 0; ElementIndex < BLOCK_SIZE * BLOCK_SIZE; ++ElementIndex)
    {
        unsigned int InIndex  = (ElementIndex % BLOCK_SIZE);
        unsigned int OutIndex = (ElementIndex / BLOCK_SIZE);
        Out[OutIndex] += In[InIndex] * Block.Elements[ElementIndex];
    }
}

void
kernel(block *Blocks, int N, int *In, int *Out)
{
#pragma HLS INTERFACE m_axi port = Blocks bundle = gmem0 offset = slave
#pragma HLS INTERFACE m_axi port = In bundle = gmem1 offset = slave
#pragma HLS INTERFACE m_axi port = Out bundle = gmem2 offset = slave

#pragma HLS INTERFACE s_axilite port = N
#pragma HLS INTERFACE s_axilite port = In
#pragma HLS INTERFACE s_axilite port = Out
#pragma HLS INTERFACE s_axilite port = return

    for(int BlockIndex = 0; BlockIndex < N; ++BlockIndex)
    {
        block Block = Blocks[BlockIndex];
        unsigned int InStart = Block.Column * BLOCK_SIZE;
        unsigned int OutStart = Block.Row * BLOCK_SIZE;

        int LocalIn[BLOCK_SIZE] = {};
        for(int I = 0; I < BLOCK_SIZE; ++I)
            LocalIn[I] = In[InStart + I];

        int LocalOut[BLOCK_SIZE] = {};
        for(int I = 0; I < BLOCK_SIZE; ++I)
            LocalOut[I] = Out[OutStart + I];

        block_spmv(Block, LocalIn, LocalOut);

        for(int I = 0; I < BLOCK_SIZE; ++I)
            Out[OutStart + I] = LocalOut[I];
    }
}