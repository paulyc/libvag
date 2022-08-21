#ifndef LIBVAG_DECODE_HPP
#define LIBVAG_DECODE_HPP

#include <cstdint>
#include <cstdio>
/*

#include "adpcm.h"

// 16 bytes -> 28 samples
const uint8_t* adpcm2pcm16le(ADPCMChannelStatus *status, const uint8_t *bytes, pcm_sample_s16le_t *samples) {
    int filter, shift, flag, byte;

    filter = *bytes++;
    shift  = filter & 0xf;
    filter = filter >> 4;
    if (filter >= 5)
        return NULL;
    flag = *bytes++;

    for (int n = 0; n < 28; n++) {
        int sample = 0, scale;

        if (flag < 7) {
            if ((n & 1) == 0) {
				byte  = *bytes++;
                scale = sign_extend(byte, 4);

            } else {
				scale = sign_extend(byte >> 4, 4);
            }

            scale  = scale << 12;
            sample = (int)((scale >> shift) + (status->sample1 * xa_adpcm_table[filter][0] + status->sample2 * xa_adpcm_table[filter][1]) / 64);
        }
        status->sample2 = status->sample1;
        status->sample1 = av_clip_int16(sample);
        set_pcm_sample_s16le(samples++, status->sample1);
    }
    return bytes;
}
*/
class AdpcmDecoder
{
public:
	AdpcmDecoder(){/*initScaleLut();*/}
	~AdpcmDecoder(){}
/*
	typedef int32_t int_pair[2];
	int_pair _scaleLut[1<<12]; // 12 bits lookup
	constexpr int lut_index(int shift, int scale){return (shift<<8)&scale;}
	void initScaleLut() {
		for (int shift=0;shift<15;++shift) {
			for (int scale=0;scale<256;++scale) {
				int s=shift>12?0:12-shift;
				_scaleLut[lut_index(shift,scale)][0] = sign_extend(scale,4)*(1<<s);
				_scaleLut[lut_index(shift,scale)][1] = sign_extend(scale>>4,4)*(1<<s);
			}
		}
	}
*/
	// 32 bytes -> 28 samples (56 bytes)
	// return number of input bytes consumed
	int decode(const uint8_t *const adpcm, int16_t *pcm){
		int shift=adpcm[0] & 0xf;
		int filter=adpcm[0] >> 4;
		if (filter >= 5) {
			fprintf(stderr,"unknown filter %d, set to 0\n",filter);
			filter=0;
		}
		if (shift > 12) {
			fprintf(stderr,"shift %d, set to 0\n",shift);
			shift=0;
		} else {
			shift = 12-shift;
		}
		int ofs_in=1;
		int ofs_out=0;
		int scale,smp;
		int flag=adpcm[ofs_in++];
		do{
			if (flag < 7) {
				scale=adpcm[ofs_in++];
				smp=sign_extend(scale,4)*(1<<shift) + ((_stat[0]*xa_adpcm_table[filter][0] + _stat[1]*xa_adpcm_table[filter][1]) >> 6);
				_stat[1] = _stat[0];
				_stat[0] = av_clip_int16(smp);
				pcm[ofs_out++]=_stat[0];
				smp=sign_extend(scale>>4,4)*(1<<shift) + ((_stat[0]*xa_adpcm_table[filter][0] + _stat[1]*xa_adpcm_table[filter][1]) >> 6);
				_stat[1] = _stat[0];
				_stat[0] = av_clip_int16(smp);
				pcm[ofs_out++]=_stat[0];
			}else{
				_stat[1] = 0;
				_stat[0] = 0;
				pcm[ofs_out++]=0;
				pcm[ofs_out++]=0;
			}
		}while(ofs_out < 28);
		return ofs_in;
	}
private:
	static constexpr int16_t av_clip_int16(int a)
	{
		if ((a+0x8000U) & ~0xFFFF) return (a>>31) ^ 0x7FFF;
		else                       return a;
	}

	static constexpr int sign_extend(int val, unsigned bits)
	{
		unsigned shift = 8 * sizeof(int) - bits;
		union { unsigned u; int s; } v = { (unsigned) val << shift };
		return v.s >> shift;
	}
	static constexpr int8_t xa_adpcm_table[5][2] = {
		{   0,   0 },
		{  60,   0 },
		{ 115, -52 },
		{  98, -55 },
		{ 122, -60 }
	};
	int _stat[2]={0};
};

#endif
