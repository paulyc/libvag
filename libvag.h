// Copyright (C) 2021 Paul Ciarlo <paul.ciarlo@gmail.com>
//
// This file is part of libvag.
//
// Foobar is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libvag is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with libvag.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdint.h>

#include "adpcm.h"

#ifndef LIBVAG_H_INCLUDED
#define LIBVAG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// BIIIIG ENDIAN, LIKE MIPS!!!
// SO IM ONLY USING CHARS SO YOU HAVE TO TRY EXTRA HARD TO F**K IT UP
// CONFLICTING SOURCES ON CHANNELS, WILL CHECK SDK
struct VAGHeader {
    char    Magick0[4]       = {'V', 'A', 'G', 'p'}; // 'VAGp' or 'pGAV'
    uint8_t Version[4]       = {0, 0, 0, 6};    // 0.6 ?
    uint8_t StartAddr[4]     = {0};
    uint8_t DataSizeBytes[4] = {0};             // FILL ME IN
    uint8_t SampleFreqHz[4]  = {0, 0, 0x7d, 0}; // 32000 Hz
    uint8_t VolumeLeft[2]    = {0, 0};
    uint8_t VolumeRight[2]   = {0, 0};
    uint8_t Pitch[2]         = {0};
    uint8_t ADSR1[2]         = {0};
    uint8_t ADSR2[2]         = {0};
    uint8_t Reserved[2]      = {0};
    char    Name[16]         = "NAMENAMENAMENAM"; // Zero Terminated or can it be 16 actual characters? What encoding? IDK
} __attribute((packed));
static_assert(sizeof(struct VAGHeader) == 0x30);

void set_vag_data_size(struct VAGHeader *hdr, uint32_t data_size_bytes);
uint32_t get_vag_data_size(struct VAGHeader *hdr);
void set_vag_sample_frequency(struct VAGHeader *hdr, uint32_t sample_frequency_hz);
char* set_vag_name(struct VAGHeader *hdr, const char *name);
FILE * init_header_open(struct VAGHeader *hdr, const char *vb_filename, bool is16k=false);

struct vag_sample {
	uint8_t raw_bytes[16]; // -> 28 output PCM samples
} __attribute__((packed));
static_assert(sizeof(struct vag_sample) == 16);
typedef struct vag_sample vag_sample_t;

#ifdef __cplusplus
}
#endif

#endif
