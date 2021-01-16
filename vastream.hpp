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

#include "adpcm.h"

class RawVAStream
{
public:
    RawVAStream();
    virtual ~RawVAStream();
    // no copy
    RawVAStream(const RawVAStream&) = delete;
    RawVAStream& operator=(const RawVAStream&) = delete;
    // move
    RawVAStream(RawVAStream&&);
    RawVAStream& operator=(RawVAStream&&);

    void Open(const std::string &filename);
    void Close();
private:
    FILE *_file;
    void *_mmap;
    size_t _map_size;
};

#endif /* VASTREAM_HPP_INCLUDED */
