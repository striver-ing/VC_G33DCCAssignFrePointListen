#pragma once
#include <stdio.h>

class WavFile {
public:
	void fwrite_int(FILE *fp, int i);
	void fwrite_uint(FILE *fp, unsigned int i);
	void fwrite_short(FILE *fp, short s);
	void WriteWavHeader(FILE *f, int m_nWavRate);
	void WriteWavEnder(FILE *fWav);
};



