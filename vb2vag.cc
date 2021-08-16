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
#include <string>
#include <cstring>
#include <cerrno>

constexpr int NumChannels = 2;
constexpr int BlockSizeBytes = 0x2000;
constexpr int BlockSizeSamples = BlockSizeBytes / sizeof(vag_sample_t);
 //NumChannels * BlockSizeSamples * sizeof(vag_sample_t);

typedef vag_sample_t channel_chunk_t[BlockSizeSamples];
typedef channel_chunk_t uninterleaved_block_t[2];

typedef vag_sample_t stereo_sample_t[2];
typedef stereo_sample_t interleaved_block_t[BlockSizeSamples];

int main(int argc, char *argv[]) {
	struct VAGHeader hdr;
	uninterleaved_block_t input_chunk;
	interleaved_block_t output_chunk;
	size_t bytes_written = 0, rd, wr, wrL, wrR;

	std::string filename;
	bool is16k=false;
	bool stereoOutput=false;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s [-t] [-s] <INPUT.VB>\n\n"
		                "Writes INPUT.VB to 0.INPUT.VB.VAG and 1.INPUT.VB.VAG (left and right channels)\n"
						"Options:\n"
						"-s Single file stereo output\n"
						"-t Talk radio (16khz sampling rate)\n\n", argv[0]);
		return ~0;
	}

	for (int i=1;i<argc;++i){
		const std::string s = std::string(argv[i]);
		if (s == "-t") {
			is16k=true;
		} else if (s == "-s") {
			//stereoOutput=true;
		} else {
			filename = s;
		}
	}

	FILE *infile = init_header_open(&hdr, filename.c_str(), is16k, false);
	FILE *outfileL = NULL;
	FILE *outfileR = NULL;
	FILE *outfile = NULL;

	fseek(infile,0,SEEK_END);
	long input_bytes = ftell(infile);
	fseek(infile,0,SEEK_SET);

	//already set by init_header_open set_vag_data_size(&hdr, input_bytes/2);
	outfileL = fopen(std::string("0." + filename + ".VAG").c_str(), "wb");
	outfileR = fopen(std::string("1." + filename + ".VAG").c_str(), "wb");
	bytes_written += fwrite(&hdr, sizeof(VAGHeader), 1, outfileL);
	bytes_written += fwrite(&hdr, sizeof(VAGHeader), 1, outfileR);

	// not sure stereo actually works with libavcodec
	outfile = fopen(std::string("2CH." + filename + ".VAG").c_str(), "wb");
	hdr.NumChannels[0] = 2;
	set_vag_data_size(&hdr, get_vag_data_size(&hdr) * 2);
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
