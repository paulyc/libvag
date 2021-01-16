/*
 * Copyright (c) 2001-2003 The FFmpeg project
 *
 * first version by Francois Revol (revol@free.fr)
 * fringe ADPCM codecs (e.g., DK3, DK4, Westwood)
 *   by Mike Melanson (melanson@pcisys.net)
 * CD-ROM XA ADPCM codec by BERO
 * EA ADPCM decoder by Robin Kay (komadori@myrealbox.com)
 * EA ADPCM R1/R2/R3 decoder by Peter Ross (pross@xvid.org)
 * EA IMA EACS decoder by Peter Ross (pross@xvid.org)
 * EA IMA SEAD decoder by Peter Ross (pross@xvid.org)
 * EA ADPCM XAS decoder by Peter Ross (pross@xvid.org)
 * MAXIS EA ADPCM decoder by Robert Marston (rmarston@gmail.com)
 * THP ADPCM decoder by Marco Gerards (mgerards@xs4all.nl)
 * Argonaut Games ADPCM decoder by Zane van Iperen (zane@zanevaniperen.com)
 * Simon & Schuster Interactive ADPCM decoder by Zane van Iperen (zane@zanevaniperen.com)
 * Ubisoft ADPCM decoder by Zane van Iperen (zane@zanevaniperen.com)
 * High Voltage Software ALP decoder by Zane van Iperen (zane@zanevaniperen.com)
 * Cunning Developments decoder by Zane van Iperen (zane@zanevaniperen.com)
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

#include "adpcm.h"

#include <stddef.h>

/**
 * Clip a signed integer value into the -32768,32767 range.
 * @param a value to clip
 * @return clipped value
 */
static inline int16_t av_clip_int16(int a)
{
    if ((a+0x8000U) & ~0xFFFF) return (a>>31) ^ 0x7FFF;
    else                       return a;
}

static inline int sign_extend(int val, unsigned bits)
{
    unsigned shift = 8 * sizeof(int) - bits;
    union { unsigned u; int s; } v = { (unsigned) val << shift };
    return v.s >> shift;
}

static const int8_t xa_adpcm_table[5][2] = {
    {   0,   0 },
    {  60,   0 },
    { 115, -52 },
    {  98, -55 },
    { 122, -60 }
};

// 16 bytes -> 28 samples
const uint8_t* adpcm2pcm(ADPCMChannelStatus *status, const uint8_t *bytes, int16_t *samples) {
    int filter, shift, flag, byte;

    filter = *bytes++;
    shift  = filter & 0xf;
    filter = filter >> 4;
    if (filter >= 5)
        return NULL;
    flag = *bytes++;
 
    /* Decode 28 samples.  */
    for (int n = 0; n < 28; n++) {
        int sample = 0, scale;

        if (flag < 0x07) {
            if (n & 1) {
                scale = sign_extend(byte >> 4, 4);
            } else {
                byte  = *bytes++;
                scale = sign_extend(byte, 4);
            }
 
            scale  = scale << 12;
            sample = (int)((scale >> shift) + (status->sample1 * xa_adpcm_table[filter][0] + status->sample2 * xa_adpcm_table[filter][1]) / 64);
        }
        *samples++ = av_clip_int16(sample);
        status->sample2 = status->sample1;
        status->sample1 = sample;
    }
    return bytes;
}
