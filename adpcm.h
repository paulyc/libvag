/*
 * Copyright (c) 2001-2003 The FFmpeg project
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * ADPCM encoder/decoder common header.
 */

#ifndef AVCODEC_ADPCM_H
#define AVCODEC_ADPCM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct ADPCMChannelStatus {
    int sample1;
    int sample2;
} ADPCMChannelStatus;

struct pcm_sample_s16le
{
    uint8_t data[2];
} __attribute__ ((packed));
typedef struct pcm_sample_s16le pcm_sample_s16le_t;

static inline void set_pcm_sample_s16le(pcm_sample_s16le_t *smp, int16_t val)
{
    smp->data[0] = (val & 0x00FF) >> 0;
    smp->data[1] = (val & 0xFF00) >> 8;
}

static inline void set_uint16_le(uint8_t out[2], uint16_t val)
{
    out[0] = (val & (0xFF <<  0)) >>  0;
    out[1] = (val & (0xFF <<  8)) >>  8;
}

static inline void set_uint16_be(uint8_t out[2], uint16_t val)
{
    out[1] = (val & (0xFF <<  0)) >>  0;
    out[0] = (val & (0xFF <<  8)) >>  8;
}

static inline void set_uint32_le(uint8_t out[4], uint32_t val)
{
    out[0] = (val & (0xFF <<  0)) >>  0;
    out[1] = (val & (0xFF <<  8)) >>  8;
    out[2] = (val & (0xFF << 16)) >> 16;
    out[3] = (val & (0xFF << 24)) >> 24;
}

static inline void set_uint32_be(uint8_t out[4], uint32_t val)
{
    out[3] = (val & (0xFF <<  0)) >>  0;
    out[2] = (val & (0xFF <<  8)) >>  8;
    out[1] = (val & (0xFF << 16)) >> 16;
    out[0] = (val & (0xFF << 24)) >> 24;
}

const uint8_t* adpcm2pcm16le(ADPCMChannelStatus *status, const uint8_t *bytes, pcm_sample_s16le_t *samples);

struct riff_chunk_desc
{
    uint8_t ChunkID[4];
    uint8_t ChunkSize[4];
    uint8_t ChunkFormat[4];
} __attribute__((packed));
typedef struct riff_chunk_desc riff_chunk_desc_t;

struct wave_fmt_desc
{
    uint8_t SubchunkID[4];
    uint8_t SubchunkSize[4];
    uint8_t AudioFormat[2];
    uint8_t NumChannels[2];
    uint8_t SampleRate[4];
    uint8_t ByteRate[4];
    uint8_t BlockAlign[2];
    uint8_t BitsPerSample[2];
} __attribute__((packed));
typedef struct wave_fmt_desc wave_fmt_desc_t;

struct wave_data_desc
{
    uint8_t SubchunkID[4];
    uint8_t SubchunkSize[4];
    // data follows
} __attribute__((packed));
typedef struct wave_data_desc wave_data_desc_t;

struct wave_file_header
{
    struct riff_chunk_desc riff;
    struct wave_fmt_desc   fmt;
    struct wave_data_desc  data;
} __attribute__((packed));
typedef struct wave_file_header wave_file_header_t;

void init_wave_file_header(wave_file_header_t *hdr, unsigned channels, unsigned sample_rate, unsigned bits_per_sample, unsigned num_samples);

#ifdef __cplusplus
}
#endif

#endif /* AVCODEC_ADPCM_H */
