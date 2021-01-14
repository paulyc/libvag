// Copyright (C) 2021 Paul Ciarlo <paul.ciarlo@gmail.com>

#include <stdio.h>
#include <stdint.h>

#ifndef LIBVAG_H_INCLUDED
#define LIBVAG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// BIIIG ENDIAN, LIKE MIPS!!!
struct VAGHeader {
    uint32_t Magick;        // 'VAGp' or 'pGAV'
    uint32_t Version;       // 0x00000006 = 0.6 ?
    uint8_t  Reserved0[4];  // 0
    uint32_t DataSizeBytes;
    uint32_t SamplingFrequencyHz;
    uint8_t  Reserved1[10]; // 0
    uint8_t  NumChannels;   // 0-1 = "mono", 2 = "stereo"
    uint8_t  Reserved2;     // 0
    char     Name[16];      // Zero Terminated or can it be 16 actual characters? What encoding? IDK
} __attribute((packed));
static_assert(sizeof(struct VAGHeader) == 0x30);

int write_vag_header(FILE *f, uint32_t data_size_bytes, uint32_t sampling_frequency_hz, uint8_t num_channels, char name[0x20]);

#ifdef __cplusplus
}
#endif

#endif
