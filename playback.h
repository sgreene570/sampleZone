#ifndef PLAYBACK_H
#define PLAYBACK_H

// Audio playback function
// @param rate: int sample rate in Hz
// @param channels: int audio channels
// @param seconds: int how much of audio file to play
// @param fd: int file descriptor
void playback(int rate, int channels, int seconds, int fd);

#endif
