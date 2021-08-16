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

#include "libvag.h"

#include <cstring>

void set_vag_data_size(struct VAGHeader *hdr, uint32_t data_size_bytes) {
    hdr->DataSizeBytes[3] = (data_size_bytes & 0xFF);
    data_size_bytes >>= 8;
    hdr->DataSizeBytes[2] = (data_size_bytes & 0xFF);
    data_size_bytes >>= 8;
    hdr->DataSizeBytes[1] = (data_size_bytes & 0xFF);
    data_size_bytes >>= 8;
    hdr->DataSizeBytes[0] = (data_size_bytes & 0xFF);
    data_size_bytes >>= 8;
}

uint32_t get_vag_data_size(struct VAGHeader *hdr) {
    return (uint32_t(hdr->DataSizeBytes[0]) << 24) |
           (uint32_t(hdr->DataSizeBytes[1]) << 16) |
           (uint32_t(hdr->DataSizeBytes[2]) <<  8) |
           (uint32_t(hdr->DataSizeBytes[3]) <<  0);
}

void set_vag_sample_frequency(struct VAGHeader *hdr, uint32_t sample_frequency_hz) {
    hdr->SampleFreqHz[3] = (sample_frequency_hz & 0xFF);
    sample_frequency_hz >>= 8;
    hdr->SampleFreqHz[2] = (sample_frequency_hz & 0xFF);
    sample_frequency_hz >>= 8;
    hdr->SampleFreqHz[1] = (sample_frequency_hz & 0xFF);
    sample_frequency_hz >>= 8;
    hdr->SampleFreqHz[0] = (sample_frequency_hz & 0xFF);
    sample_frequency_hz >>= 8;
}

char* set_vag_name(struct VAGHeader *hdr, const char *name) {
    return strncpy(hdr->Name, name, sizeof(hdr->Name));
}

FILE* init_header_open(struct VAGHeader *hdr, const char *vb_filename, bool is16k, bool stereoOutput) {
    FILE *f = fopen(vb_filename, "rb");
    fseek(f, 0, SEEK_END);
    uint32_t data_size_bytes = ftell(f);
    set_vag_data_size(hdr, data_size_bytes/(stereoOutput?1:2));
    if (is16k) {
        set_vag_sample_frequency(hdr, 16000);
    }
    set_vag_name(hdr, vb_filename);
    fseek(f, 0, SEEK_SET);
    return f;
}
