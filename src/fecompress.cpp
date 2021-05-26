#include "fecompress.h"
#include <vector>
#include <algorithm>
#include <cstdint>
//#include <iostream>

int FECompress::DecompressData(uint8_t*& output, uint8_t *input)
{
    std::vector<uint8_t> data_buffer;
    uint8_t* input_position = input;
    int saved_counter = 0;
    uint8_t* saved_position = input;
    unsigned char adr_03, adr_04 = 0;
    char counter = 0;
    bool done = false;
    while (!done) {
        unsigned int buffer_position = data_buffer.size();
        unsigned char control = *input_position++;
        if (saved_counter != 0) {
            saved_counter--;
            if (saved_counter == 0) {
                input_position = saved_position;
            }
        }
        if ( (control & 0x80) == 0) {
            if ( (control & 0x40) != 0) {
                if (control >= 0x60) {
                    // 80:B6BA
                    // control code: 0x60
                    adr_03 = *input_position++;
                    if (saved_counter !=0) {
                        saved_counter--;
                        if (saved_counter == 0) {
                            input_position = saved_position;
                        }
                    }
                    counter = (control&0x0F)+1;
                    if (control >= 0x70) {
                        // 80:B6FC
                        // control code: 0x70
                        // Alternating (changing byte first)
                        for (int i = counter; i >=0 ; i--) {
                            data_buffer.push_back(*input_position++);
                            data_buffer.push_back(adr_03);
                            if (saved_counter !=0) {
                                saved_counter--;
                                if (saved_counter == 0) {
                                    input_position = saved_position;
                                }
                            }
                        }
                    } else {
                        // 80:B6DD
                        // control code: 0x60
                        // Alternating (changin byte second)
                        for (int i = counter; i >=0 ; i--) {
                            data_buffer.push_back(adr_03);
                            data_buffer.push_back(*input_position++);
                            if (saved_counter !=0) {
                                saved_counter--;
                                if (saved_counter == 0) {
                                    input_position = saved_position;
                                }
                            }
                        }
                    }
                } else if (control >= 0x50) {
                    // 80:B693
                    // control code = 0x50
                    // pairs of bytes (AAAABBBBCCCC...), max 16 pairs
                    counter = control&0x0F;
                    for (int i = counter; i>=0; i--) {
                        control = *input_position++;
                        if (saved_counter !=0) {
                            saved_counter--;
                            if (saved_counter == 0) {
                                input_position = saved_position;
                            }
                        }
                        data_buffer.push_back(control);
                        data_buffer.push_back(control);
                    }
                } else {
                    // 80:B5FF
                    // control code = 0x40
                    // Alternating nibbles between all 0's/1's and unique information
                    control &= 0x0F;
                    control++;
                    counter = control;
                    control = *(input_position++);
                    if (saved_counter !=0) {
                        saved_counter--;
                        if (saved_counter == 0) {
                            input_position = saved_position;
                        }
                    }
                    if (control >=0x80) {
                        // second control contains data
                        // 80:B5BA
                        adr_03 = control;
                        control <<= 1;
                        if ((control & 0x80) != 0) {
                            // if 0x40 is set, then use all 1s for the fixed nibble
                            control &= 0x20;
                            if (control != 0) {
                                // secondary control code = 0xD0
                                control = adr_03 << 4;
                                control |= 0x0F;
                                data_buffer.push_back(control);
                                control = 0x1F;
                            } else {
                                // secondary control code = 0xC0
                                control = adr_03 & 0x0F;
                                control |= 0xF0;
                                data_buffer.push_back(control);
                                control = 0x0F;
                            }
                        } else {
                            // if 0x40 is not set, then use all 0s for the fixed nibble
                            control &= 0x20;
                            if (control != 0) {
                                // secondary control code = 0x90
                                control = adr_03 << 4;
                                data_buffer.push_back(control);
                                control = 0x10;
                            } else {
                                // secondary control code = 0x80
                                control = (adr_03 & 0x0F);
                                data_buffer.push_back(control);
                                control = 0;
                            }
                        }
                    }
                    if (control >=0x10) {
                        // 80:B622
                        // Unique nibble stored in the high nibble
                        adr_03 = control & 0x0F;
                        while (counter >= 0) {
                            control = *input_position++;
                            if (saved_counter != 0) {
                               saved_counter--; if(saved_counter == 0){ input_position = saved_position; }
                            }
                            adr_04 = control;
                            control &= 0xF0;
                            control |= adr_03;
                            data_buffer.push_back(control);
                            counter--;
                            if (counter >= 0) {
                                control = adr_04;
                                control <<= 4;
                                control |= adr_03;
                                data_buffer.push_back(control);
                            }
                            counter--;
                        }
                    } else {
                        // 80:B656
                        // Unique nibble stored in the low nibble
                        adr_03 = control << 4;
                        while (counter >= 0) {
                            control = *input_position++;
                            if(saved_counter != 0) {
                                saved_counter--;
                                if (saved_counter == 0) {
                                    input_position = saved_position;
                                }
                            }
                            adr_04 = control;
                            control >>=4;
                            control |= adr_03;
                            data_buffer.push_back(control);
                            counter--;
                            if (counter >=0) {
                                control = adr_04 & 0x0F;
                                control |= adr_03;
                                data_buffer.push_back(control);
                            }
                            counter--;
                        }
                    }
                }
            } else {
                // 80:B71D
                // control code = 0x00
                // Uncompressed information, max 0x3F
                for (int i = 0; i <= control; i++) {
                    data_buffer.push_back(*input_position++);
                }
                if (saved_counter != 0) {
                    saved_counter--;
                    if (saved_counter == 0) {
                        input_position = saved_position;
                    }
                }
            }
        } else {
            if ((control & 0x40) != 0) {
                if (control < 0xE0) {
                    // 80:B78C
                    // Control code 0xC0
                    // read from previously decompressed information., max length 0x20 (0x41?)
                    // max difference
                    // sequence appears to be: xxxCCCCC CDDDDDDD DDDDDDDD
                    //( c = counted, D - difference)
                    counter = (control & 0x1F);
                    control = *input_position++;
                    unsigned short difference = (unsigned short)counter;
                    difference <<= 8;
                    if (saved_counter != 0) {
                        saved_counter--;
                        if (saved_counter == 0) {
                            input_position = saved_position;
                        }
                    }
                    difference |= (unsigned short)control;
                    difference <<= 1;
                    counter = (unsigned char) (difference >> 8) + 1;
                    difference <<= 8;
                    difference >>= 1;
                    difference |= *input_position++;
                    int bufPos = buffer_position - difference;
                    for (int i = counter; i >= 0; i--) {
                        data_buffer.push_back(data_buffer[bufPos]);
                        bufPos++;
                    }
                    if (saved_counter != 0) {
                        saved_counter--;
                        if (saved_counter == 0) {
                            input_position = saved_position;
                        }
                    }
                } else if (control < 0xF0) {
                    // 80:B7B5
                    // Control code 0xE0
                    // write a single byte, using a word counter.
                    // min is 3, but if it's shorter than 10 should probably use byte counter.
                    control &= 0x0F;
                    adr_04 = control;
                    unsigned short wCounter = (unsigned short) control;
                    wCounter <<= 8;
                    control = *input_position++;
                    if (saved_counter != 0) {
                        saved_counter--;
                        if (saved_counter == 0) {
                           input_position = saved_position;
                        }
                    }
                    wCounter |= control;
                    wCounter += 3;
                    bool odd = wCounter & 0x0001;
                    wCounter >>= 1;
                    control = *input_position++;
                    for (int i = 0; i < wCounter; i++) {
                        data_buffer.push_back(control);
                        data_buffer.push_back(control);
                    }
                    if (odd) {
                        data_buffer.push_back(control);
                    }
                    if(saved_counter != 0) {
                        saved_counter--;
                        if (saved_counter == 0) {
                            input_position = saved_position;
                        }
                    }
                }
                else if (control < 0xF8) {
                    // 80:B7F7
                    // Control code 0xF0
                    // write signle byte, using a byte counter.
                    // Min 3, max 10
                    counter = (control&0x07)+2;
                    control = *input_position++;
                    for ( int i = counter; i>=0; i--) {
                        data_buffer.push_back(control);
                    }
                    if (saved_counter != 0) {
                        saved_counter--; 
                        if (saved_counter == 0) { 
                            input_position = saved_position; 
                        }
                    }
                } else if (control < 0xFC) {
                    // 80:B814
                    // Control code 0xF8
                    // repeat at least three previous compressed sections
                    // stored difference is based on position before reading this sections codes
                    // which is why 3 is added to it.
                    // xxxxxxCC CCCDDDDD DDDDDDDD
                    unsigned char counter = control&0x03;
                    counter <<= 3;
                    counter += 0x03;
                    unsigned int difference = *input_position++;
                    counter += (difference >> 5);
                    difference &= 0x1F;
                    difference <<=8;
                    difference |= *input_position++;
                    difference += 0x03;

                    saved_position = input_position ;
                    input_position = input_position - difference;
                    saved_counter = counter;
                } else if (control < 0xFE) {
                    // 80:B86D
                    // repeat 3 or 4 previous compressed sections
                    // stored difference is based on position before reading this sections codes
                    // which is why 2 is added to it.
                    // xxxxxxxxC CDDDDDDD
                    // Counter = c x 2
                    unsigned short difference = (control & 0x01) << 8;
                    control = *input_position++;
                    difference |= control;
                    unsigned char highbyte = (difference << 2) >> 8;
                    difference = (difference & 0x3f) + 2;

                    saved_position = input_position ;
                    input_position = input_position - difference;
                    saved_counter = highbyte + 3;

                } else {
                    // 80:B88C
                    done = true;
                }
            } else {
                // 80:B752
                // Control code 0x80
                // read from already decompressed info
                // xxccccdd dddddddd
                // counter = (c >> 2) +1
                // max counter = 17?
                // max difference = 03FF ?
                control &= 0x7F;
                short difference = (short) control;
                counter = (unsigned char) (control >> 2) + 1;
                difference &= 0x0003;
                difference  <<= 8;
                difference |= *input_position++;
                int bufPos = buffer_position - difference;
                for (int i = counter; i >= 0; i--) {
                    control = data_buffer[bufPos];
                    data_buffer.push_back(control);
                    bufPos++;
                }
                if (saved_counter != 0) {
                   saved_counter--; 
                   if (saved_counter == 0) { 
                       input_position = saved_position; 
                   }
                }
            }
        }
    }
    output = new uint8_t[data_buffer.size()];
    std::copy(data_buffer.begin(), data_buffer.end(), output);
    return data_buffer.size();
}
