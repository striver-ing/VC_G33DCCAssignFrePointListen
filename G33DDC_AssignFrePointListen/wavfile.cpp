#pragma once
#include "wavfile.h"

void WavFile::fwrite_int(FILE *fp, int i)
{
#ifndef BIGENDIAN
	fwrite(&i, 4, 1, fp);
#else
	unsigned char buf[4];
	bury_int(buf, i);
	fwrite(&buf, 1, 4, fp);
#endif
}

void WavFile::fwrite_uint(FILE *fp, unsigned int i)
{
#ifndef BIGENDIAN
	fwrite(&i, 4, 1, fp);
#else
	unsigned char buf[4];
	bury_uint(buf, i);
	fwrite(&buf, 1, 4, fp);
#endif
}

void WavFile::fwrite_short(FILE *fp, short s)
{
#ifndef BIGENDIAN
	fwrite(&s, 2, 1, fp);
#else
	unsigned char buf[4];
	bury_int(buf, s);
	fwrite(&buf, 1, 2, fp);
#endif
}
void WavFile::WriteWavHeader(FILE *f, int m_nWavRate)
{

	if (f != NULL)
	{
		short word;
		int dword;

		fwrite("RIFF", 4, 1, f);
		dword = 0;
		fwrite_int(f, dword);

		fwrite("WAVEfmt ", 8, 1, f);
		dword = 16;
		fwrite_int(f, dword);
		word = 1;
		fwrite_short(f, word);
		word = 1;
		fwrite_short(f, word);
		dword = m_nWavRate;
		fwrite_int(f, dword);
		dword = m_nWavRate * 2;
		fwrite_int(f, dword);
		word = 2;
		fwrite_short(f, word);
		word = 16;
		fwrite_short(f, word);

		fwrite("data", 4, 1, f);
		dword = 0;
		fwrite_int(f, dword);
	}
}
void WavFile::WriteWavEnder(FILE *fWav)
{
	if (fWav != NULL)
	{
		int dword;
		int len;
		fseek(fWav, 0, SEEK_END);
		len = ftell(fWav);

		fseek(fWav, 4, SEEK_SET);
		dword = (len - 8);
		fwrite_int(fWav, dword);

		fseek(fWav, 40, SEEK_SET);
		dword = (len - 44);
		fwrite_int(fWav, dword);
		fclose(fWav);
	}

}

