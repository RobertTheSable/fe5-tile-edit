#ifndef FECOMPRESS_H
#define FECOMPRESS_H

namespace FECompress
{
    int CompressData(unsigned char*& output, unsigned char* input, int input_size);
    int DecompressData(unsigned char*& output, unsigned char* input);
}

#endif // FECOMPRESS_H
