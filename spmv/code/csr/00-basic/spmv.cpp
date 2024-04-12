#define BUFFER_SIZE 256

extern "C" {

    void spmv()
    // void spmv(int *RowPointers, int RowPointersLength, int *Columns,int RowPointersLength, dtype *Elements,int RowPointersLength, dtype *VectorIn, dtype VectorOut, int RowPointersLength)
    {
#if 0
        #pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem0
        #pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem1
        #pragma HLS INTERFACE m_axi port=c offset=slave bundle=gmem0

        #pragma HLS INTERFACE s_axilite port=a bundle=control
        #pragma HLS INTERFACE s_axilite port=b bundle=control
        #pragma HLS INTERFACE s_axilite port=c bundle=control

        #pragma HLS INTERFACE s_axilite port=return bundle=control
        #pragma HLS INTERFACE s_axilite port=n_elements bundle=control

        int arrayA[BUFFER_SIZE];
        int arrayB[BUFFER_SIZE];
        int arrayC[BUFFER_SIZE];

    main_loop:
        for (int i = 0; i < n_elements; i += BUFFER_SIZE)
        {
            int size = BUFFER_SIZE;

            if(i + size > n_elements)
                size = n_elements - i;

        readA:
            for(int j = 0; j < size; j++)
                arrayA[j] = a[i + j];

        readB:
            for(int j = 0; j < size; j++)
                arrayB[j] = b[i + j];

        vadd:
            for(int j = 0; j < size; j++)
                arrayC[j] = arrayA[j] + arrayB[j];
        
        writeC:
        for(int j = 0; j < size; j++)
             c[i + j] = arrayC[j];

        }
#endif
    }
}