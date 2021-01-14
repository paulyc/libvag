#include "libvag.h"
#include <string>

int main(int argc, char *argv[]) {
	char *filename;
	bool is16k;

	if (argc == 3) {
		filename = argv[2];
		is16k = true;
	} else if (argc == 2) {
		filename = argv[1];
		is16k = false;
	} else {
		fprintf(stderr, "Usage: %s [-t] <INPUT.VB>\n\n"
		                "Writes INPUT.VB to stdout with a VAG header prepended, assuming it is all data\n"
						"Options:\n"
						"-t Talk radio (16khz sampling rate)\n\n", argv[0]);
		return ~0;
	}

	size_t written = prepend_header_and_dump(filename, is16k);
	fprintf(stderr, "Wrote %zu bytes\n", written);
	return 0;
}
