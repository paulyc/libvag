CXX ?= g++
CXXFLAGS := $(CXXFLAGS) -g

all: libvag.a libvag.so vb2vag

libvag.so: libvag.so.0.0.0
	ln -sf libvag.so.0.0.0 libvag.so.0.0
	ln -sf libvag.so.0.0.0 libvag.so.0
	ln -sf libvag.so.0.0.0 libvag.so

libvag.a: libvag.o
	ar rcs libvag.a libvag.o

libvag.so.0.0.0: libvag.lo
	g++ $(LDFLAGS) -shared -fPIC libvag.lo -o libvag.so.0.0.0

libvag.o: libvag.cc
	$(CXX) $(CXXFLAGS) -c libvag.cc -o libvag.o

libvag.lo: libvag.cc
	$(CXX) $(CXXFLAGS) -fPIC -c libvag.cc -o libvag.lo

vb2vag.o: vb2vag.cc
	$(CXX) $(CXXFLAGS) -c vb2vag.cc -o vb2vag.o

vb2vag: vb2vag.o
	g++ vb2vag.o libvag.a -o vb2vag

clean:
	rm -fv libvag.so libvag.so.0 libvag.so.0.0 libvag.so.0.0.0 libvag.a libvag.o libvag.lo vb2vag.o vb2vag
.PHONY: clean
