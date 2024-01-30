#include "kernel.h"
#include "stdio.h"

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