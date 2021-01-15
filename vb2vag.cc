#include "libvag.h"
#include <string>
#include <cstring>
#include <cerrno>

struct vag_sample_t {
	uint8_t raw_bytes[16]; // -> 28 output PCM samples
} __attribute__((packed));
static_assert(sizeof(vag_sample_t) == 16);

constexpr int NumChannels = 2;
constexpr int BlockSizeBytes = 0x2000;
constexpr int BlockSizeSamples = BlockSizeBytes / sizeof(vag_sample_t);
 //NumChannels * BlockSizeSamples * sizeof(vag_sample_t);

typedef vag_sample_t channel_chunk_t[BlockSizeSamples];
typedef channel_chunk_t uninterleaved_block_t[2];

typedef vag_sample_t stereo_sample_t[2];
typedef stereo_sample_t interleaved_block_t[BlockSizeSamples];

#if 0
/*
 * Copyright (c) 2001-2003 The FFmpeg project
 * adpcm.c
 */

void adpcm2pcm() {
	for (channel = 0; channel < avctx->channels; channel++) {
        samples = samples_p[channel];
 
             /* Read in every sample for this channel.  */
             for (i = 0; i < nb_samples / 28; i++) {
                 int filter, shift, flag, byte;
 
                 filter = bytestream2_get_byteu(&gb);
                 shift  = filter & 0xf;
                 filter = filter >> 4;
                 if (filter >= FF_ARRAY_ELEMS(xa_adpcm_table))
                     return AVERROR_INVALIDDATA;
                 flag   = bytestream2_get_byteu(&gb);
 
                 /* Decode 28 samples.  */
                 for (n = 0; n < 28; n++) {
                     int sample = 0, scale;
 
                     if (flag < 0x07) {
                         if (n & 1) {
                             scale = sign_extend(byte >> 4, 4);
                         } else {
                             byte  = bytestream2_get_byteu(&gb);
                             scale = sign_extend(byte, 4);
                         }
 
                         scale  = scale << 12;
                         sample = (int)((scale >> shift) + (c->status[channel].sample1 * xa_adpcm_table[filter][0] + c->status[channel].sample2 * xa_adpcm_table[filter][1]) / 64);
                     }
                     *samples++ = av_clip_int16(sample);
                     c->status[channel].sample2 = c->status[channel].sample1;
                     c->status[channel].sample1 = sample;
                 }
             }
         }
         break
}
#endif

int main(int argc, char *argv[]) {
	struct VAGHeader hdr;
	uninterleaved_block_t input_chunk;
	interleaved_block_t output_chunk;
	size_t bytes_written = 0, rd, wr, wrL, wrR;

	std::string filename;
	bool is16k;

	if (argc == 3) {
		filename = argv[2];
		is16k = true;
	} else if (argc == 2) {
		filename = argv[1];
		is16k = false;
	} else {
		fprintf(stderr, "Usage: %s [-t] <INPUT.VB>\n\n"
		                "Writes INPUT.VB to 0.INPUT.VB.VAG and 1.INPUT.VB.VAG (left and right channels)\n"
						"Options:\n"
						"-t Talk radio (16khz sampling rate)\n\n", argv[0]);
		return ~0;
	}

	FILE *infile = init_header_open(&hdr, filename.c_str(), is16k);
	FILE *outfileL = fopen(std::string("0." + filename + ".VAG").c_str(), "wb");
	FILE *outfileR = fopen(std::string("1." + filename + ".VAG").c_str(), "wb");
	FILE *outfile = fopen(std::string("2CH." + filename + ".VAG").c_str(), "wb");
	bytes_written += fwrite(&hdr, sizeof(VAGHeader), 1, outfileL);
	bytes_written += fwrite(&hdr, sizeof(VAGHeader), 1, outfileR);
	set_vag_data_size(&hdr, 2 * get_vag_data_size(&hdr));
	fwrite(&hdr, sizeof(VAGHeader), 1, outfile);
    for (; !feof(infile); ) {
		wr = 0;
        rd = fread(input_chunk, sizeof(vag_sample_t), NumChannels * BlockSizeSamples, infile);
		if (rd <= 0) {
			break;
		}

		for (size_t i = 0; i < rd / 2; ++i) {
			wrL = fwrite(&input_chunk[0][i], sizeof(vag_sample_t), 1, outfileL);
			wrR = fwrite(&input_chunk[1][i], sizeof(vag_sample_t), 1, outfileR);
			fwrite(&input_chunk[0][i], sizeof(vag_sample_t), 1, outfile);
			fwrite(&input_chunk[1][i], sizeof(vag_sample_t), 1, outfile);
			if (wrL <= 0 || wrR <= 0) {
				break;
			} else {
				wr += (wrL + wrR);
			}
		}

		if (wrL <= 0 || wrR <= 0) {
			break;
		}
		if (wr != rd) {
			fprintf(stderr, "wr %zu != rd %zu\n", wr, rd);
		}
		bytes_written += wr * sizeof(vag_sample_t);
    }
	int err = errno;
	fclose(infile);
	fclose(outfileL);
	fclose(outfileR);
	fclose(outfile);
	fprintf(stderr, "Wrote %zu bytes\n", bytes_written);
	if (err) {
		fprintf(stderr, "errno set : [%d] %s\n", err, strerror(err));
	}
	return err;
}
