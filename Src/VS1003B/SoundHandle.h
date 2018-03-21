#ifndef __SOUND_HANDLE_H__
#define __SOUND_HANDLE_H__

#define SOUND_USE_ALLOC		0

#define SOUND_CLOSE_FILE_ON		1
#define SOUND_CLOSE_FILE_OFF	0

#define SOUND_EXT_MP3		1
#define SOUND_EXT_WAV		2

#define SOUND_BUFFER_SIZE	512

#define SOUND_VOLUME_STEP	10
#define SOUND_VOLUME_DEF	1
#define SOUND_VOLUME_UP		2
#define SOUND_VOLUME_DN		3

typedef enum {
	SOUND_OK,
	SOUND_END,
	SOUND_ALLOC_ERR,
	SOUND_ERR,
	SOUND_OPEN_FAIL,
	SOUND_READ_FAIL
} sound_result_t;

typedef enum {
	SOUND_PLAY_INIT,
	SOUND_PLAY_PLAYING,
	SOUND_PLAY_PAUSE,
	SOUND_PLAY_STOPPED,
	SOUND_PLAY_NONE
} play_state_t;

typedef enum {
	PLAY_BUFFER_1 = 0x01,
	PLAY_BUFFER_2 = 0x02,
	PLAY_BUFFER_END = 0x04,
	PLAY_STOP = 0x10,
	PLAY_SOUND_END = 0x20,
	PLAY_SOUND_CPLT = 0x40,
	PLAY_NONE = 0x80
} buffering_state_t;

typedef struct {
	buffering_state_t play_state;
	int data_size;
	TCHAR * buffer;
} audio_msg_t;


void Sound_VolOff(void);
uint8_t Sound_GetVolume(void);
void Sound_ControlVolume(uint8_t updn);
void Sound_SetVolume(uint8_t vol);
int Sound_Init(char * f_path);
void Sound_Deinit(int close_file);
sound_result_t Sound_PlayStart(void);
sound_result_t Sound_Buffering(void);
sound_result_t Sound_Playing(void);
play_state_t GetPlayState(void);
void SetPlayState(play_state_t state);
int MemAlloc(void);

#endif /* __SOUND_HANDLE_H__ */
