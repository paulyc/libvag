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
#include "vastream.hpp"
#include <string>
#include <optional>
#include <cstring>
#include <cerrno>

void usage(char *procname) {
	fprintf(stderr, "Usage: %s [-t|--talk] INPUT.VB [OUTPUT.RAW]\n\n"
		            "Converts INPUT.VB to stereo 16-bit signed PCM; writes to OUTPUT.RAW, or stdout\n"
					"Options:\n"
					"-t --talk Talk radio (16khz sampling rate)\n\n", procname);
}

int main(int argc, char *argv[]) {
	std::optional<std::string> filename;
	std::optional<std::string> outfilename;
	FILE *outfile = stdout;
	bool is16k = false;
    VCVAStream vcstream;
    VCVAStreamLQ vcstreamlq;
    int smp_written;

	for (char **arg = argv + 1; arg < argv + argc; ++arg) {
		std::string strarg(*arg);
		if (strarg == "-t" || strarg == "--talk") {
			is16k = true;
		} else if (strarg == "-h" || strarg == "--help" || strarg == "--usage") {
			usage(argv[0]);
			return ~0;
		} else if (filename == std::nullopt) {
			filename = strarg;
		} else if (outfilename == std::nullopt) {
			outfilename = strarg;
		} else {
			usage(argv[0]);
			return ~0;
		}
	}

	if (outfilename != std::nullopt) {
		outfile = fopen(outfilename.value().c_str(), "wb");
	} else {
		// needs to know number of samples in advance so can't use stdin but could be fixed
		usage(argv[0]);
		return ~0;
	}

    if (is16k) {
        vcstreamlq.Open(filename.value());
        smp_written = vcstreamlq.DumpRawPCM(outfile);
        vcstreamlq.Close();
    } else {
        vcstream.Open(filename.value());
        smp_written = vcstream.DumpRawPCM(outfile);
        vcstream.Close();
    }
	if (outfile != stdout) {
		fclose(outfile);
	}
    fprintf(stderr, "Wrote %d samples\n", smp_written);
    return 0;
}
