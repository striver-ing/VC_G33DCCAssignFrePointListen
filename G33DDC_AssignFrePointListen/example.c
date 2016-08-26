#include "G33DDCAPI.h"
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>

#define MSG_EXITTHREAD (WM_APP+1000)

G33DDC_OPEN_DEVICE OpenDevice;
G33DDC_CLOSE_DEVICE CloseDevice;
G33DDC_SET_POWER SetPower;
G33DDC_SET_DDC1 SetDDC1;
G33DDC_GET_DDC1 GetDDC1;
G33DDC_GET_DDC2 GetDDC2;
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

HMODULE hAPI=NULL;
HANDLE hThread;
DWORD ThreadId;
HWAVEOUT hWaveOut;
UINT32 BufferCount;


//procedure of thread used for freeing audio buffers returned to application
//from audio subsystem
DWORD __stdcall WaveOutThreadProcedure(void *Param)
{
 MSG Msg;
 WAVEHDR *WaveHdr;

    while(GetMessage(&Msg,NULL,0,0))
    {
        switch(Msg.message)
        {
            case MSG_EXITTHREAD:
                //exit thread
                return 0;

            case MM_WOM_DONE:
                WaveHdr=(WAVEHDR*)Msg.lParam;

                waveOutUnprepareHeader(hWaveOut,WaveHdr,sizeof(WAVEHDR));

                VirtualFree(WaveHdr,0,MEM_RELEASE);

                BufferCount--;
                break;

            default:
                DispatchMessage(&Msg);
                break;
        }
    }

    return 0;
}

BOOL Initialize(void)
{
 WAVEFORMATEX WaveFormat;

    hAPI=LoadLibrary("G33DDCAPI.dll");

    if(hAPI==NULL)
    {
        printf("Failed to load G33DDCAPI.dll.\n");
        return FALSE;
    }

    //retrieve addresses of used API functions
    OpenDevice=(G33DDC_OPEN_DEVICE)GetProcAddress(hAPI,"OpenDevice");
    CloseDevice=(G33DDC_CLOSE_DEVICE)GetProcAddress(hAPI,"CloseDevice");
    SetPower=(G33DDC_SET_POWER)GetProcAddress(hAPI,"SetPower");
    SetDDC1=(G33DDC_SET_DDC1)GetProcAddress(hAPI,"SetDDC1");
    GetDDC1=(G33DDC_GET_DDC1)GetProcAddress(hAPI,"GetDDC1");
    GetDDC2=(G33DDC_GET_DDC2)GetProcAddress(hAPI,"GetDDC2");    
    SetCallbacks=(G33DDC_SET_CALLBACKS)GetProcAddress(hAPI,"SetCallbacks");
    SetFrequency=(G33DDC_SET_FREQUENCY)GetProcAddress(hAPI,"SetFrequency");
    GetFrequency=(G33DDC_GET_FREQUENCY)GetProcAddress(hAPI,"GetFrequency");
    StartDDC1=(G33DDC_START_DDC1)GetProcAddress(hAPI,"StartDDC1");
    StopDDC1=(G33DDC_STOP_DDC1)GetProcAddress(hAPI,"StopDDC1");
    StartDDC2=(G33DDC_START_DDC2)GetProcAddress(hAPI,"StartDDC2");
    StopDDC2=(G33DDC_STOP_DDC2)GetProcAddress(hAPI,"StopDDC2");
    StartAudio=(G33DDC_START_AUDIO)GetProcAddress(hAPI,"StartAudio");
    StopAudio=(G33DDC_STOP_AUDIO)GetProcAddress(hAPI,"StopAudio");
    SetAGC=(G33DDC_SET_AGC)GetProcAddress(hAPI,"SetAGC");
    SetAGCParams=(G33DDC_SET_AGC_PARAMS)GetProcAddress(hAPI,"SetAGCParams");
    SetDemodulatorMode=(G33DDC_SET_DEMODULATOR_MODE)GetProcAddress(hAPI,"SetDemodulatorMode");
    SetDemodulatorFilterBandwidth=(G33DDC_SET_DEMODULATOR_FILTER_BANDWIDTH)GetProcAddress(hAPI,"SetDemodulatorFilterBandwidth");
    SetDDC1Frequency=(G33DDC_SET_DDC1_FREQUENCY)GetProcAddress(hAPI,"SetDDC1Frequency");
    SetDDC2Frequency=(G33DDC_SET_DDC2_FREQUENCY)GetProcAddress(hAPI,"SetDDC2Frequency");
    SetDemodulatorFrequency=(G33DDC_SET_DEMODULATOR_FREQUENCY)GetProcAddress(hAPI,"SetDemodulatorFrequency");
    GetDDC1Frequency=(G33DDC_GET_DDC1_FREQUENCY)GetProcAddress(hAPI,"GetDDC1Frequency");
    GetDDC2Frequency=(G33DDC_GET_DDC2_FREQUENCY)GetProcAddress(hAPI,"GetDDC2Frequency");
    GetDemodulatorFrequency=(G33DDC_GET_DEMODULATOR_FREQUENCY)GetProcAddress(hAPI,"GetDemodulatorFrequency");

    //open the first available G33DDC device
    hDevice=OpenDevice(G3XDDC_OPEN_FIRST);

    if(hDevice<0)
    {
        printf("No available G33DDC device found.\n");
        return FALSE;
    }


    //create thread which frees audio buffers returned to application
    hThread=CreateThread(NULL,0,WaveOutThreadProcedure,NULL,0,&ThreadId);

    if(hThread==NULL)
    {
        printf("Failed to create thread. Error=%u\n",GetLastError());
        CloseDevice(hDevice);
        FreeLibrary(hAPI);
        return FALSE;
    }

    //initialize waveout
    WaveFormat.wFormatTag=WAVE_FORMAT_PCM;
    WaveFormat.nChannels=1;
    WaveFormat.nSamplesPerSec=48000;
    WaveFormat.wBitsPerSample=16;
    WaveFormat.nBlockAlign=WaveFormat.nChannels*WaveFormat.wBitsPerSample/8;
    WaveFormat.nAvgBytesPerSec=WaveFormat.nBlockAlign*WaveFormat.nSamplesPerSec;
    WaveFormat.cbSize=0;

    //reset buffer count
    BufferCount=0;

    if(waveOutOpen(&hWaveOut,WAVE_MAPPER,&WaveFormat,ThreadId,NULL,CALLBACK_THREAD)!=MMSYSERR_NOERROR)
    {
        printf("Failed to open waveout\n");
        PostThreadMessage(ThreadId,MSG_EXITTHREAD,0,0);
        WaitForSingleObject(hThread,INFINITE);
        CloseHandle(hThread);
        CloseDevice(hDevice);
        FreeLibrary(hAPI);
        return FALSE;
    }

    return TRUE;
}

void __stdcall MyDDC2PreprocessedStreamCallback(UINT32 Channel,CONST FLOAT *Buffer,UINT32 NumberOfSamples,FLOAT SlevelPeak,FLOAT SlevelRMS,DWORD_PTR UserData)
{
 double Slevel_dBm;

    //convert slevel RMS in Volts to dBm
    Slevel_dBm=10.0*log10(SlevelRMS*SlevelRMS*(1000.0/50.0));

    printf("\rSlevel: RMS[V]=%9.7f V, RMS[dBm]=%6.1f dBm, Peak[V]=%9.7f V",SlevelRMS,Slevel_dBm,SlevelPeak);
}

void __stdcall MyAudioStreamCallback(UINT32 Channel,CONST FLOAT *Buffer,CONST FLOAT *BufferFiltered,UINT32 NumberOfSamples,DWORD_PTR UserData)
{
 CONST FLOAT *Input;
 WAVEHDR *WaveHdr;
 SHORT *Output;

    //alocate memory for waveout audio buffer and WAVEHDR structure
    WaveHdr=(WAVEHDR*)VirtualAlloc(NULL,sizeof(WAVEHDR)+NumberOfSamples*sizeof(SHORT),MEM_COMMIT,PAGE_READWRITE);

    ZeroMemory(WaveHdr,sizeof(WAVEHDR));

    WaveHdr->lpData=(LPSTR)(WaveHdr+1); //audio buffer starts after WAVEHDR structure
    WaveHdr->dwBufferLength=NumberOfSamples*sizeof(SHORT);

    //retype Buffer to INT32, because bits per sample of DDC type 1 (specified by SetDDCType) is 32 - signed
    Input=(CONST FLOAT*)Buffer;

    //retype audio buffer ptr to signed 16bit
    Output=(SHORT*)WaveHdr->lpData;

    //convert samples from 32bit IEEE float to 16bit integer
    while(NumberOfSamples--)
    {
        *Output++=(SHORT)((*Input++)*32767.0);
    }

    waveOutPrepareHeader(hWaveOut,WaveHdr,sizeof(WAVEHDR));

    //increase number of buffers sent to waveout
    BufferCount++;

    if(waveOutWrite(hWaveOut,WaveHdr,sizeof(WAVEHDR))!=MMSYSERR_NOERROR)
    {
        //decrease number of buffers sent to waveout because an error
        BufferCount--;
        VirtualFree(WaveHdr,0,MEM_RELEASE);
    }
}

int main(int argc, char* argv[])
{
 G3XDDC_DDC_INFO DDCInfo;
 UINT32 DDCTypeIndex;
 G33DDC_CALLBACKS Callbacks;
 UINT32 Frequency;
 UINT32 DDC1Frequency;
 INT32 DDC2Frequency;
 INT32 DemodulatorFrequency;

    if(!Initialize())
    {
        return -1;
    }

    //set power
    SetPower(hDevice,TRUE);

    //set DDC type of DDC1 to 1 (DDC Bandwidth=24000, SampleRate=32kHz, BitsPerSample=32)
    SetDDC1(hDevice,1);

    //retrieve current DDC type of the DDC1
    GetDDC1(hDevice,&DDCTypeIndex,&DDCInfo);
    printf("DDC type of DDC1: Index=%u, Bandwidth=%u, SampleRate=%u, BitsPerSample=%u\n",DDCTypeIndex,DDCInfo.Bandwidth,DDCInfo.SampleRate,DDCInfo.BitsPerSample);

    //retrieve current DDC type of the DDC2
    GetDDC2(hDevice,&DDCTypeIndex,&DDCInfo);
    printf("DDC type of DDC2: Index=%u, Bandwidth=%u, SampleRate=%u, BitsPerSample=%u\n",DDCTypeIndex,DDCInfo.Bandwidth,DDCInfo.SampleRate,DDCInfo.BitsPerSample);

    //IFCallbacks is not required in this example
    Callbacks.IFCallback=NULL;

    //DDC1StreamCallback is not required in this example
    Callbacks.DDC1StreamCallback=NULL;

    //DDC1PlaybackStreamCallback is not required in this example
    Callbacks.DDC1PlaybackStreamCallback=NULL;

    //DDC2StreamCallback is not required in this example
    Callbacks.DDC2StreamCallback=NULL;

    //DDC2StreamCallback is not required in this example
    Callbacks.DDC2StreamCallback=NULL;

    //DDC2PreprocessedStreamCallback is used to display signal level in this example
    Callbacks.DDC2PreprocessedStreamCallback=MyDDC2PreprocessedStreamCallback;

    //AudioStreamCallback is used to play audio on wave out device
    Callbacks.AudioStreamCallback=MyAudioStreamCallback;

    //AudioPlaybackStreamCallback is not used in this example
    Callbacks.AudioPlaybackStreamCallback=NULL;

    //register callback functiobs
    SetCallbacks(hDevice,&Callbacks,0);

    //tune channel 0 to an AM station frequency
    SetFrequency(hDevice,0,702000);
    //it is possible to tune DDC1, DDC2 and demodulator directly using
    //SetDDC1Frequency, SetDDC2Frequency and SetDemodulatorFrequency
    //SetFrequency(hDevice,0,702000) can be replaced by the following:
    //SetDDC1Frequency(hDevice,700000);
    //SetDDC2Frequency(hDevice,0,0);
    //SetDemodulatorFrequency(hDevice,0,2000);
    //final absolute frequency of the demodulator is 700000+0+2000=702000

    //get absolute frequency where the demodulator of channel 0 is tuned
    GetFrequency(hDevice,0,&Frequency);

    //get DDC1 frequency
    GetDDC1Frequency(hDevice,&DDC1Frequency);

    //get relative DDC2 frequency for channel 0
    GetDDC2Frequency(hDevice,0,&DDC2Frequency);

    //get relative demodulator frequency for channel 0
    GetDemodulatorFrequency(hDevice,0,&DemodulatorFrequency);

    printf("Listening to (demodulator absolute frequency): %u Hz\n",Frequency);
    printf("DDC1 frequency: %u Hz\n",DDC1Frequency);
    printf("DDC2 frequency (relative): %d Hz\n",DDC2Frequency);
    printf("Demodulator frequency (relative): %d Hz\n",DemodulatorFrequency);


    //set AGC params for channel 0, attack=0.02s, decay=0.5s, ref. level=-15dB
    SetAGCParams(hDevice,0,0.020,0.500,-15.0);

    //enable AGC for channel 0
    SetAGC(hDevice,0,TRUE);

    //set AM mode for channel 0
    SetDemodulatorMode(hDevice,0,G3XDDC_MODE_AM);

    //set demodulator filter bandwidth for channel 0 to 8kHz
    SetDemodulatorFilterBandwidth(hDevice,0,8000);

    //start DDC1 streaming which has to be running before StartDDC2 is called
    StartDDC1(hDevice,1024);

    printf("\nPress ENTER to exit.\n\n");    

    //start DDC2 streaming for channel 0, it has to be running before StartAudio is called
    StartDDC2(hDevice,0,1024);


    //start audio streaming for channel 0, with 1024 samples per buffer
    StartAudio(hDevice,0,1024);

    getchar();

    //stop audio streaming for channel 0
    StopAudio(hDevice,0);

    //stop DDC2 streaming for channel 0
    StopDDC2(hDevice,0);

    //stop DDC1 streaming
    StopDDC1(hDevice);

    //in this case it is not necessary to use StopAudio and StopDDC2
    //because StopDDC1 stop audio and DDC2 streaming too


    waveOutReset(hWaveOut);

    //wait for all buffers sent to waveout are returned
    while(BufferCount)
    {
        Sleep(10);
    }

    PostThreadMessage(ThreadId,MSG_EXITTHREAD,0,0);
    WaitForSingleObject(hThread,INFINITE);
    CloseHandle(hThread);

    waveOutClose(hWaveOut);

    //unregister all callback functions
    SetCallbacks(hDevice,NULL,0);

    SetPower(hDevice,FALSE);

    CloseDevice(hDevice);

    FreeLibrary(hAPI);

    return 0;
}
