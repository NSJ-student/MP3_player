#ifndef __AUDIO_FILE_HANDLE_H__
#define __AUDIO_FILE_HANDLE_H__

#define AUDIO_FILE_FAIL		0

#pragma pack(1)
typedef struct {
	uint8_t id[3];
	uint8_t version[2];
	uint8_t flags;
	uint32_t size;
//	uint8_t size[4];
} MP3_ID3v2;

typedef struct {
	uint8_t id[4];
	uint32_t size;
	uint16_t flags;
} MP3_ID3v2_Ex;

typedef struct {
	uint32_t sync : 11;
	uint32_t version :2;
	uint32_t layer :2;
	uint32_t protect :1;
	uint32_t bitrate :4;
	uint32_t samplerate :2;
	uint32_t padding :1;
	uint32_t priv_bit :1;
	uint32_t channel :2;
	uint32_t mode :2;
	uint32_t copyright :1;
	uint32_t original :1;
	uint32_t emphasis :2;
} MP3_Frame_Header;

typedef struct {
	uint8_t ChunkID[4];
	uint32_t ChunkSize;
	uint8_t Format[4];
} RIFF;

typedef struct {
	uint8_t ChunkID[4];
	uint32_t ChunkSize;
	uint16_t AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t AvgByteRate;
	uint16_t BlockAligh;
	uint16_t BitPerSample;
} FMT;

typedef struct {
	uint8_t ChunkID[4];
	uint32_t ChunkSize;
} DATA;


typedef struct {
	RIFF Riff;
	FMT Fmt;
	DATA Data;
} WAV_Header;

#pragma pack(4)

#define SWAP32(x)	((((x)&0xFF)<<24) | (((x)&0xFF00)<<8) | (((x)&0xFF0000)>>8) | (((x)&0xFF000000)>>24))
#define SWAP16(x)	((((x)&0xFF)<<8) | (((x)&0xFF00)>>8))

#define ID3v2_SIZE_POS			6
#define ID3v2_FLAGS_POS			5
#define FOOTER_PRESEND_FLAG	0x10

uint32_t MP3_FindStartPos(FIL * ptrFile);
uint32_t MP3_FindFileLen(FIL * ptrFile, uint32_t mp3_start_pos);
float WVA_CalPlayTime(FIL * ptrFile);


#endif
