#pragma once

#include "G33DDCAPI.h"
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <time.h>
#include "wavfile.h"

#define MSG_EXITTHREAD (WM_APP+1000)

class Controller {
public:
	BOOL Initialize(const char* serialNumber, int channel);
	void stopRecording();
	bool isReachTime();
	void doTask(const char* serialNumber, int channel, int frequence, const char* filePath, int fileTotalTime);
	static void __stdcall MyDDC2PreprocessedStreamCallback(UINT32 Channel, CONST FLOAT *Buffer, UINT32 NumberOfSamples, FLOAT SlevelPeak, FLOAT SlevelRMS, DWORD_PTR UserData);
	static void __stdcall MyAudioStreamCallback(UINT32 Channel, CONST FLOAT *Buffer, CONST FLOAT *BufferFiltered, UINT32 NumberOfSamples, DWORD_PTR UserData);

private:
	G33DDC_GET_DEVICE_LIST GetDeviceList;
	G33DDC_OPEN_DEVICE OpenDevice;
	G33DDC_CLOSE_DEVICE CloseDevice;
	G33DDC_SET_POWER SetPower;
	G33DDC_SET_DDC1 SetDDC1;
	G33DDC_GET_DDC1 GetDDC1;
	G33DDC_GET_DDC2 GetDDC2;
	G33DDC_GET_DDC_INFO GetDDCInfo;
	G33DDC_SET_CALLBACKS SetCallbacks;
	G33DDC_SET_FREQUENCY SetFrequency;
	G33DDC_GET_FREQUENCY GetFrequency;
	G33DDC_START_DDC1 StartDDC1;
	G33DDC_STOP_DDC1 StopDDC1;
	G33DDC_START_DDC2 StartDDC2;
	G33DDC_STOP_DDC2 StopDDC2;
	G33DDC_START_AUDIO StartAudio;
	G33DDC_STOP_AUDIO StopAudio;
	G33DDC_SET_AGC SetAGC;
	G33DDC_SET_AGC_PARAMS SetAGCParams;
	G33DDC_SET_DEMODULATOR_MODE SetDemodulatorMode;
	G33DDC_SET_DEMODULATOR_FILTER_BANDWIDTH SetDemodulatorFilterBandwidth;
	G33DDC_SET_DDC1_FREQUENCY SetDDC1Frequency;
	G33DDC_SET_DDC2_FREQUENCY SetDDC2Frequency;
	G33DDC_SET_DEMODULATOR_FREQUENCY SetDemodulatorFrequency;
	G33DDC_GET_DDC1_FREQUENCY GetDDC1Frequency;
	G33DDC_GET_DDC2_FREQUENCY GetDDC2Frequency;
	G33DDC_GET_DEMODULATOR_FREQUENCY GetDemodulatorFrequency;


	INT32 hDevice;

	HMODULE hAPI = NULL;
	HANDLE hThread;
	DWORD ThreadId;
	HWAVEOUT hWaveOut;
	UINT32 BufferCount;

	FILE* fpWav = NULL;
	time_t  m_RecordingTime;
	bool isBeginRecording = 0;
	DWORDLONG m_dwlRecordingTime = 0;

	WavFile *wavFile;

	//void setMyself()
	//{
	//	MyCtrl = this;
	//}

};
