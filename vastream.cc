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

#include "vastream.hpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <exception>
#include <cstdio>
#include <cerrno>

RawVAStream::RawVAStream() : _file(nullptr), _mmap(nullptr), _map_size(0)
{
}

// lol C++
RawVAStream::RawVAStream(RawVAStream &&other) : _file(other._file), _mmap(other._mmap), _map_size(other._map_size)
{
    other._file = nullptr;
    other._mmap = nullptr;
    other._map_size = 0;
}

RawVAStream& RawVAStream::operator=(RawVAStream &&other)
{
    this->_file = other._file;
    other._file = nullptr;
    this->_mmap = other._mmap;
    other._mmap = nullptr;
    this->_map_size = other._map_size;
    other._map_size = 0;
    return *this;
}

RawVAStream::~RawVAStream()
{
    Close();
}

void RawVAStream::Open(const std::string &filename)
{
    struct stat s;
    int res = stat(filename.c_str(), &s);
    if (res != 0 || s.st_size <= 0) {
        throw std::exception();
    }
    _map_size = s.st_size;

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
void RawVAStream::Close()
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
