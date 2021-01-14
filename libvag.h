// Copyright (C) 2021 Paul Ciarlo <paul.ciarlo@gmail.com>

#include <stdio.h>
#include <stdint.h>

#ifndef LIBVAG_H_INCLUDED
#define LIBVAG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// BIIIIG ENDIAN, LIKE MIPS!!!
// SO IM ONLY USING CHARS SO YOU HAVE TO TRY EXTRA HARD TO F**K IT UP
struct VAGHeader {
    char    Magick[4]        = {'V', 'A', 'G', 'p'}; // 'VAGp' or 'pGAV'
    uint8_t Version[4]       = {0, 0, 0, 6};    // 0.6 ?
    uint8_t Reserved0[4]     = {0};
    uint8_t DataSizeBytes[4] = {0};             // FILL ME IN
    uint8_t SampleFreqHz[4]  = {0, 0, 0x7d, 0}; // 32000 Hz
    uint8_t Reserved1[10]    = {0};
    uint8_t NumChannels      =  2;              // 0-1 = "mono", 2 = "stereo"
    uint8_t Reserved2        =  0;
    char    Name[16]         = "NAMENAMENAMENAM"; // Zero Terminated or can it be 16 actual characters? What encoding? IDK
} __attribute((packed));
static_assert(sizeof(struct VAGHeader) == 0x30);

void set_vag_data_size(struct VAGHeader *hdr, uint32_t data_size_bytes);
void set_vag_sample_frequency(struct VAGHeader *hdr, uint32_t sample_frequency_hz);
char* set_vag_name(struct VAGHeader *hdr, char *name);
size_t prepend_header_and_dump(char *vb_filename);

#ifdef __cplusplus
}
#endif

#endif
