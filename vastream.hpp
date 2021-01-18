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

#ifndef VASTREAM_HPP_INCLUDED
#define VASTREAM_HPP_INCLUDED

#include <cstdio>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <exception>
#include <cstdio>
#include <cerrno>

#include "libvag.h"

template <unsigned SampleRate, off_t BlockSizeBytes, int Channels=2>
class RawVAStream
{
public:
    RawVAStream() :
        _file(nullptr),
        _mmap(nullptr),
        _map_size(0),
        _chanstat{{0,0}}
    {}
    virtual ~RawVAStream()
    {
        Close();
    }
    // no copy
    RawVAStream(const RawVAStream&) = delete;
    RawVAStream& operator=(const RawVAStream&) = delete;
    // no move
    RawVAStream(RawVAStream&&) = delete;
    RawVAStream& operator=(RawVAStream&&) = delete;

    void Open(const std::string &filename)
    {
        struct stat s;
        int res = stat(filename.c_str(), &s);
        if (res != 0 || s.st_size <= 0) {
            throw std::exception();
        }
        _map_size = s.st_size;

        //nb_samples = buf_size / (16 * ch) * 28;

        _file = fopen(filename.c_str(), "rb");
        if (_file == NULL) {
            throw std::exception();
        }

        _mmap = mmap(0, _map_size, PROT_READ, MAP_SHARED, fileno(_file), 0);
        if (_mmap == MAP_FAILED) {
            _mmap = nullptr;
            _map_size = 0;
        }
    }

    static constexpr off_t BlockSizeSamples = BlockSizeBytes / sizeof(vag_sample_t);
    typedef vag_sample_t channel_chunk_t[BlockSizeSamples];
    typedef channel_chunk_t uninterleaved_block_t[Channels];

    typedef vag_sample_t stereo_sample_t[Channels];
    typedef stereo_sample_t interleaved_block_t[BlockSizeSamples];

    static constexpr off_t BlockSizePcmSamples = 28 * BlockSizeSamples;
    typedef pcm_sample_s16le_t pcm_block_t[BlockSizePcmSamples];
    typedef pcm_block_t uninterleaved_pcm_block_t[Channels];

    constexpr inline off_t adpcmSamples() const
    {
        return _map_size / (sizeof(struct vag_sample) * Channels);
    }

    constexpr inline off_t pcmSamples() const
    {
        return 28 * adpcmSamples();
    }

    int DumpWAV(FILE *outfile)
    {
        wave_file_header_t hdr;
        init_wave_file_header(&hdr, 2, 32000, 16, pcmSamples());
        fwrite(&hdr, sizeof(wave_file_header_t), 1, outfile);
        return DumpRawPCM(outfile);
    }

    int DumpRawPCM(FILE *outfile)
    {
        uninterleaved_pcm_block_t out;
        const uint8_t *blk = static_cast<const uint8_t*>(_mmap);
        const off_t adpcmWholeBlocks = adpcmSamples() / BlockSizeSamples;
        const off_t adpcmPartialBlockSize = adpcmSamples() % BlockSizeSamples;
        const off_t pcmPartialBlockSamples = 28 * adpcmPartialBlockSize;
        off_t adpcmOffset = 0;
        off_t smpWritten = 0;
        for (off_t adpcmBlock = 0; adpcmBlock < adpcmWholeBlocks; ++adpcmBlock) {
            for (int ch = 0; ch < Channels; ++ch) {
                for (int smp = 0; smp < BlockSizeSamples; ++smp) {
                    blk = adpcm2pcm16le(_chanstat + ch, blk, &out[ch][smp * 28]);
                }
            }
            for (int smp = 0; smp < BlockSizePcmSamples; ++smp) {
                for (int ch = 0; ch < Channels; ++ch) {
                    smpWritten += fwrite(&out[ch][smp], sizeof(pcm_sample_s16le_t), 1, outfile);
                }
            }
        }
        for (int ch = 0; ch < Channels; ++ch) {
            for (int smp = 0; smp < adpcmPartialBlockSize; ++smp) {
                blk = adpcm2pcm16le(_chanstat + ch, blk, &out[ch][smp * 28]);
            }
        }
        for (int smp = 0; smp < pcmPartialBlockSamples; ++smp) {
            for (int ch = 0; ch < Channels; ++ch) {
                smpWritten += fwrite(&out[ch][smp], sizeof(pcm_sample_s16le_t), 1, outfile);
            }
        }
        return smpWritten;
    }
    void Close()
    {
        if (_mmap != nullptr && _mmap != MAP_FAILED) {
            munmap(_mmap, _map_size);
            _mmap = nullptr;
            _map_size = 0;
        }

        if (_file != nullptr) {
            fclose(_file);
            _file = nullptr;
        }
    }

private:
    FILE *_file;
    void *_mmap;
    off_t _map_size;
    ADPCMChannelStatus _chanstat[Channels];
};

typedef RawVAStream<32000, 0x2000, 2> VCVAStream;
typedef RawVAStream<16000, 0x2000, 2> VCVAStreamLQ;

#endif /* VASTREAM_HPP_INCLUDED */
