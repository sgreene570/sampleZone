#ifndef PLAYBACK_H
#define PLAYBACK_H

// Audio playback function
// @param rate: int sample rate in Hz
// @param channels: int audio channels
// @param seconds: int how much of audio file to play
// @param fd: int file descriptor
void playback(unsigned int rate, int channels, double seconds, int fd);

// WAV file header spec as per Msft standard
typedef struct wavHeader_t {
    u_int32_t chunkID;
    u_int32_t chunkSize;
    u_int32_t format;
    u_int32_t subChunk1ID;
    u_int32_t subChunk1Size;
    u_int16_t audioFormat;
    u_int16_t numChannels;
    u_int32_t sampleRate;
    u_int32_t byteRate;
    u_int16_t blockAlign;
    u_int16_t bitsPerSample;
    u_int32_t subChunk2ID;
    u_int32_t subChunk2Size;
} wavHeader;

// Audio file basic struct
typedef struct audioFile_t {
    char *fileName;
    int pitchAdjust;
} audioFile;

#endif
