#ifndef WAVEFILE_H
#define WAVEFILE_H

#endif // WAVEFILE_H

typedef struct WavFormatHeader
{
    char RIFF[4];
    long Filesize;
    char WavFlags[4];
    char FMTFlags[4];
    int ByteFilter;
    short Format;
    short Channel;
    long SampleFreq;
    long ByteFreq;
    short BlockAlign;
    short PCMBitSize;
    char fact[4];
    long Datasize;
}WaveFormat;
typedef struct Chuck
{

}DataChuck;

//波的状态是向上还是向下
enum wavestate{UP,DOWN,FLAT};
//波的顶点是波峰还是波谷
enum pole{CREST,TROUGH};
typedef struct
{
    int begin;
    int end;
}Sec;
