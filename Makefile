
CC ?= gcc
CFLAGS := $(CFLAGS) -g -std=c11

CXX ?= g++
CXXFLAGS := $(CXXFLAGS) -g -std=c++17

all: libvag.a libvag.so vb2vag vb2pcm

libvag.a: libvag.o adpcm.o
	ar rcs libvag.a libvag.o adpcm.o

libvag.so.0.0.0: libvag.lo adpcm.lo
	g++ $(LDFLAGS) -shared -fPIC libvag.lo adpcm.lo -o libvag.so.0.0.0

libvag.so: libvag.so.0.0.0
	ln -sf libvag.so.0.0.0 libvag.so.0.0
	ln -sf libvag.so.0.0.0 libvag.so.0
	ln -sf libvag.so.0.0.0 libvag.so

libvag.o: libvag.cc libvag.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

libvag.lo: libvag.cc libvag.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

vastream.o: vastream.cc vastream.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

vastream.lo: vastream.cc vastream.hpp
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

adpcm.o: adpcm.c adpcm.h
	$(CC) $(CFLAGS) -c $< -o $@

adpcm.lo: adpcm.c adpcm.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

vb2vag.o: vb2vag.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

vb2vag.lo: vb2vag.cc
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

vb2pcm.o: vb2pcm.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

vb2pcm.lo: vb2pcm.cc
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

vb2vag: vastream.o vb2vag.o libvag.a
	g++ vastream.o vb2vag.o libvag.a -o vb2vag

vb2pcm: vastream.o vb2pcm.o libvag.a
	g++ vastream.o vb2pcm.o libvag.a -o vb2pcm

clean:
	rm -fv *.o *.lo vb2vag vb2pcm libvag.a libvag.so libvag.so.*
.PHONY: clean

install: vb2pcm vb2vag
	install -m0755 vb2pcm vb2vag /usr/local/bin/

#for file in *.VB; do ffmpeg -i "vags/0.$file.VAG" -i "vags/1.$file.VAG" -filter_complex "[0:a][1:a]amerge=inputs=2[a]" -map "[a]"  -y "wavs/$file.WAV"; done


