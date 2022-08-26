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
	AdpcmDecoder(){}
	~AdpcmDecoder(){}

	// 32 bytes -> 28 samples (56 bytes)
	// return number of input bytes consumed
	int decode(const uint8_t *const adpcm, int16_t *pcm){
		int shift=adpcm[0] & 0xf;
		int filter=adpcm[0] >> 4;
		int flag=adpcm[1];
		int ofs_in=2;
		int ofs_out=0;
		int scale,smp;

		if (filter >= 5) {
			fprintf(stderr,"filter unknown %d, set to 0\n",filter);
			filter=0;
		}
		if (shift > 12) {
			fprintf(stderr,"shift %d, set to 0\n",shift);
			shift=0;
		} else {
			shift = 12-shift;
		}
		if (flag >= 7){
			fprintf(stderr,"flag unknown %d, ignoring\n",flag);
		}

		do{
			scale=adpcm[ofs_in++];
			smp=(_x=scale)*(1<<shift) + ((_s1*xa_adpcm_table[filter][0] + _s2*xa_adpcm_table[filter][1]) >> 6);
			_s2 = _s1;
			_s1 = av_clip_int16(smp);
			pcm[ofs_out++]=_s1;
			smp=(_x=(scale>>4))*(1<<shift) + ((_s1*xa_adpcm_table[filter][0] + _s2*xa_adpcm_table[filter][1]) >> 6);
			_s2 = _s1;
			_s1 = av_clip_int16(smp);
			pcm[ofs_out++]=_s1;
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
		unsigned const shift = 8 * sizeof(int) - bits;
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
	int _s1=0;
	int _s2=0;
	struct {signed int _x:4;};
};

#endif
