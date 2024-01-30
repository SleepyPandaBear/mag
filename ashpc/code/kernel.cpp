#include "kernel.h"
#include "stdio.h"

#if 0
void
block_spmv(const block Block, const int In[], int Out[])
{
    l_block_spmv:
    for(unsigned int ElementIndex = 0; ElementIndex < BLOCK_SIZE * BLOCK_SIZE; ++ElementIndex)
    {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=8
        unsigned int InIndex  = (ElementIndex % BLOCK_SIZE);
        unsigned int OutIndex = (ElementIndex / BLOCK_SIZE);
        Out[OutIndex] += In[InIndex] * Block.Elements[ElementIndex];
    }
}

void
kernel(const block *Blocks, const int N, const int *In, int *Out)
{
#pragma HLS INTERFACE m_axi port = Blocks bundle = gmem0 offset = slave
#pragma HLS INTERFACE m_axi port = In bundle = gmem1 offset = slave
#pragma HLS INTERFACE m_axi port = Out bundle = gmem2 offset = slave

#pragma HLS INTERFACE s_axilite port = Blocks
#pragma HLS INTERFACE s_axilite port = N
#pragma HLS INTERFACE s_axilite port = In
#pragma HLS INTERFACE s_axilite port = Out
#pragma HLS INTERFACE s_axilite port = return

    l_blocks:
    for(int BlockIndex = 0; BlockIndex < N; ++BlockIndex)
    {
        #pragma HLS PIPELINE II=1
        block Block = Blocks[BlockIndex];
        unsigned int InStart = Block.Column * BLOCK_SIZE;
        unsigned int OutStart = Block.Row * BLOCK_SIZE;

        int LocalIn[BLOCK_SIZE] = {};
        l_read_vec_in:
        for(int I = 0; I < BLOCK_SIZE; ++I)
        {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=8
            LocalIn[I] = In[InStart + I];
        }

        int LocalOut[BLOCK_SIZE] = {};
        l_read_vec_out:
        for(int I = 0; I < BLOCK_SIZE; ++I)
        {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=8
            LocalOut[I] = Out[OutStart + I];
        }

        block_spmv(Block, LocalIn, LocalOut);

        l_write_vec_out:
        for(int I = 0; I < BLOCK_SIZE; ++I)
        {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=8
            Out[OutStart + I] = LocalOut[I];
        }
    }
}
#endif

void
kernel_block_spmv(int Elements[], int In[], int Out[])
{
#pragma HLS INTERFACE m_axi port = Elements bundle = gmem1 offset = slave
#pragma HLS INTERFACE m_axi port = In bundle = gmem2 offset = slave
#pragma HLS INTERFACE m_axi port = Out bundle = gmem3 offset = slave

#pragma HLS INTERFACE s_axilite port = Elements
#pragma HLS INTERFACE s_axilite port = In
#pragma HLS INTERFACE s_axilite port = Out
#pragma HLS INTERFACE s_axilite port = return

    int ElementsBuffer[BLOCK_SIZE*BLOCK_SIZE];
    l_read_elements:
    for(unsigned int I = 0; I < BLOCK_SIZE * BLOCK_SIZE; ++I)
        ElementsBuffer[I] = Elements[I];

    int InBuffer[BLOCK_SIZE];
    l_read_in:
    for(unsigned int I = 0; I < BLOCK_SIZE; ++I)
        InBuffer[I] = In[I];

    int OutBuffer[BLOCK_SIZE];
    l_read_out:
    for(unsigned int I = 0; I < BLOCK_SIZE; ++I)
        OutBuffer[I] = Out[I];

    l_block_spmv:
    for(unsigned int ElementIndex = 0; ElementIndex < BLOCK_SIZE * BLOCK_SIZE; ++ElementIndex)
    {
        unsigned int InIndex  = (ElementIndex % BLOCK_SIZE);
        unsigned int OutIndex = (ElementIndex / BLOCK_SIZE);
        OutBuffer[OutIndex] += InBuffer[InIndex] * ElementsBuffer[ElementIndex];
    }
}