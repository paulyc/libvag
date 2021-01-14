#include "libvag.h"

#include <cstring>

void set_vag_data_size(struct VAGHeader *hdr, uint32_t data_size_bytes) {
    hdr->DataSizeBytes[0] = data_size_bytes & (0xFF << 24);
    hdr->DataSizeBytes[1] = data_size_bytes & (0xFF << 16);
    hdr->DataSizeBytes[2] = data_size_bytes & (0xFF << 8);
    hdr->DataSizeBytes[3] = data_size_bytes & (0xFF << 0);
}

void set_vag_sample_frequency(struct VAGHeader *hdr, uint32_t sample_frequency_hz) {
    hdr->SampleFreqHz[0] = sample_frequency_hz & (0xFF << 24);
    hdr->SampleFreqHz[1] = sample_frequency_hz & (0xFF << 16);
    hdr->SampleFreqHz[2] = sample_frequency_hz & (0xFF << 8);
    hdr->SampleFreqHz[3] = sample_frequency_hz & (0xFF << 0);
}

char* set_vag_name(struct VAGHeader *hdr, char *name) {
    return strncpy(hdr->Name, name, sizeof(hdr->Name));
}

size_t prepend_header_and_dump(char *vb_filename) {
    size_t written = 0;
    char buf[4096];
    struct VAGHeader hdr;
    FILE *f = fopen(vb_filename, "rb");
    fseek(f, 0, SEEK_END);
    uint32_t data_size_bytes = ftell(f);
    set_vag_data_size(&hdr, data_size_bytes);
    fseek(f, 0, SEEK_SET);
    written += fwrite(&hdr, 1, sizeof(VAGHeader), stdout);
    while (!feof(f)) {
        size_t rd = fread(buf, sizeof(buf), sizeof(char), f);
        written += fwrite(buf, sizeof(buf), sizeof(char), stdout);
    }
    fclose(f);
    return written;
}
