#include "mp3.h"


int initMp3Module(char *buffer){

	printf("initialisiere mp3 Decoder...\n");
	HMP3Decoder mp3Dec = MP3InitDecoder();

	if(mp3Dec == NULL){
		printf("ERROR Initialisierung des mp3 Decoder fehlgeschlagen!\n");
	}else{
		printf("mp3 Decoder initialisiert!\n");
	}
	
	int nextSyncWord = MP3FindSyncWord((unsigned char*)buffer, 2048);

    printf("nextSyncWord = %d\n",nextSyncWord);	


	//void MP3GetLastFrameInfo(HMP3Decoder hMP3Decoder,MP3FrameInfo *mp3FrameInfo);

	printf("MP3FreeDecoder()...\n");
	MP3FreeDecoder(mp3Dec);	
	
	return 0;

}

