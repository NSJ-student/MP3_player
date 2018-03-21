#include "includes.h"

char audio_len[11];
static uint32_t DecStrToInt(char * str, uint32_t len);
	
static uint32_t DecStrToInt(char * str, uint32_t len)
{
	uint32_t result = 0;
	uint32_t mul = 1;
	
	do
	{
		len--;
		result += (str[len] - '0') * mul;
		mul = mul*10;
	}while(len);
	
	return result;
}

uint32_t MP3_FindStartPos(FIL * ptrFile)
{
	MP3_ID3v2 mp3_hdr;
	uint32_t mp3_start_pos;
	uint32_t readByte;
	
	if(FR_OK != User_f_read (ptrFile, &mp3_hdr, sizeof(MP3_ID3v2), &readByte))
	{
		return AUDIO_FILE_FAIL;
	}
	
	mp3_start_pos = 10;	// ID3v2_FLAGS_POS Header size
	mp3_start_pos += SWAP32(mp3_hdr.size);			// extended header size
	if((mp3_hdr.flags)&FOOTER_PRESEND_FLAG)	// footer size
	{
		mp3_start_pos += 10;
	}
	
	return mp3_start_pos;
}

uint32_t MP3_FindFileLen(FIL * ptrFile, uint32_t mp3_start_pos)
{
	osPoolDef (MemPool, 1, MP3_ID3v2_Ex);
	osPoolId   MemPool_Id;
	osStatus   status;
	
	MP3_ID3v2_Ex * mp3_ex_hdr;
	uint32_t curPos = 10;
	uint32_t readByte, size;
	
	MemPool_Id = osPoolCreate (osPool (MemPool));
	if (MemPool_Id != NULL)  
	{
		mp3_ex_hdr = (MP3_ID3v2_Ex *)osPoolCAlloc (MemPool_Id);
		if (mp3_ex_hdr == NULL)
			return AUDIO_FILE_FAIL;
	}
	else
		return AUDIO_FILE_FAIL;
	
	User_f_lseek(ptrFile, curPos);
	
	while(curPos < mp3_start_pos)
	{
		if(FR_OK != User_f_read (ptrFile, mp3_ex_hdr, sizeof(MP3_ID3v2_Ex), &readByte))
		{
			osPoolFree (MemPool_Id, mp3_ex_hdr);
			return AUDIO_FILE_FAIL;
		}
		curPos += readByte;
		size = SWAP32(mp3_ex_hdr->size);
		if(size == 0)
			break;
		if(!memcmp(mp3_ex_hdr->id, "TLEN", 4))
		{
			if(FR_OK != User_f_read (ptrFile, audio_len, size, &readByte))
			{
				osPoolFree (MemPool_Id, mp3_ex_hdr);
				return AUDIO_FILE_FAIL;
			}
			curPos += readByte;
			
			audio_len[readByte] = 0;
			GUI_SetFileLen(audio_len);
			
			osPoolFree (MemPool_Id, mp3_ex_hdr);
			
			return DecStrToInt(audio_len, size);
		}
		else
		{
			curPos += size;
			if(FR_OK != User_f_lseek (ptrFile, curPos))
			{
				osPoolFree (MemPool_Id, mp3_ex_hdr);
				return AUDIO_FILE_FAIL;
			}
		}
	}
	
	osPoolFree (MemPool_Id, mp3_ex_hdr);
	
	return AUDIO_FILE_FAIL;
}

uint32_t MP3_CalPlayTime(FIL * ptrFile, uint32_t mp3_start_pos)
{
	osPoolDef (MemPool, 1, MP3_Frame_Header);
	osPoolId   MemPool_Id;
	osStatus   status;
	
	MP3_Frame_Header * frame_hdr;
	uint32_t curPos = 10;
	uint32_t readByte, size;
	
	MemPool_Id = osPoolCreate (osPool (MemPool));
	if (MemPool_Id != NULL)  
	{
		frame_hdr = (MP3_Frame_Header *)osPoolCAlloc (MemPool_Id);
		if (frame_hdr == NULL)
			return AUDIO_FILE_FAIL;
	}
	else
		return AUDIO_FILE_FAIL;
	
	User_f_lseek(ptrFile, mp3_start_pos);
	
	if(FR_OK != User_f_read (ptrFile, frame_hdr, sizeof(MP3_Frame_Header), &readByte))
	{
		osPoolFree (MemPool_Id, frame_hdr);
		return AUDIO_FILE_FAIL;
	}
	// Duration = Number of Frames * Samples Per Frame / Sampling Rate
	
	osPoolFree (MemPool_Id, frame_hdr);
	
	return AUDIO_FILE_FAIL;
}

float WVA_CalPlayTime(FIL * ptrFile)
{
	WAV_Header wva_hdr;
	uint32_t Duration;
	uint32_t readByte, size;
	
	User_f_lseek(ptrFile, 0);
	if(FR_OK != User_f_read (ptrFile, &wva_hdr, sizeof(WAV_Header), &readByte))
	{
		return AUDIO_FILE_FAIL;
	}
	// Duration = Number of Frames * Samples Per Frame / Sampling Rate
	Duration = (ptrFile->obj.objsize - sizeof(WAV_Header));
	size = wva_hdr.Fmt.BitPerSample * wva_hdr.Fmt.SampleRate;
	if(size != 0)
	{
		return (8 * Duration / size);
	}
	
	return AUDIO_FILE_FAIL;
}


#if 0
uint32_t MP3_NumberOfFrame(FIL * ptrFile, uint32_t mp3_start_pos)
{
	osPoolDef (MemPool, 1, MP3_ID3v2_Ex);
	osPoolId   MemPool_Id;
	osStatus   status;
	
	MP3_Frame_Header * frame_hdr;
	uint32_t curPos = 10;
	uint32_t readByte, size;
	
	MemPool_Id = osPoolCreate (osPool (MemPool));
	if (MemPool_Id != NULL)  
	{
		frame_hdr = (MP3_Frame_Header *)osPoolCAlloc (MemPool_Id);
		if (frame_hdr == NULL)
			return AUDIO_FILE_FAIL;
	}
	else
		return AUDIO_FILE_FAIL;
	
	User_f_lseek(ptrFile, curPos);
	
	if(FR_OK != User_f_read (ptrFile, frame_hdr, sizeof(MP3_Frame_Header), &readByte))
	{
		osPoolFree (MemPool_Id, frame_hdr);
		return AUDIO_FILE_FAIL;
	}
	// Duration = Number of Frames * Samples Per Frame / Sampling Rate
	
	osPoolFree (MemPool_Id, frame_hdr);
	
	return AUDIO_FILE_FAIL;
}
#endif
