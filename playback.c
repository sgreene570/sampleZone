/*
 * Simple sound playback using ALSA API and libasound.
 *
 * Copyright (C) 2009 Alessandro Ghedini <alessandro@ghedini.me>
 * --------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Alessandro Ghedini wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we
 * meet some day, and you think this stuff is worth it, you can
 * buy me a beer in return.
 * --------------------------------------------------------------
 * Refactored/improved by Stephen Greene (sgreene570)
 */

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <alloca.h>
#include "playback.h"

#include <ncurses.h>
#include "ncursesUtils.h"

#define PCM_DEVICE "default"

void playback(unsigned int rate, int channels, double seconds, int fd){
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames;
    unsigned int tmp;
    int buff_size, loops, pcm;
    char *buff;

    /* Open the PCM device in playback mode */
    if ((pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
                    SND_PCM_STREAM_PLAYBACK, 0)) < 0)
        printf("ERROR: Can't open \"%s\" PCM device. %s\n",
                    PCM_DEVICE, snd_strerror(pcm));

    /* Allocate parameters object and fill it with default values*/
    snd_pcm_hw_params_alloca(&params);

    snd_pcm_hw_params_any(pcm_handle, params);

    /* Set parameters */
    if ((pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
                    SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
        //printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));
        sampleError("Can't set interleaved mode.", DEFAULT_WINDOW_HEIGHT);

    if ((pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
                        SND_PCM_FORMAT_S16_LE)) < 0)
        //printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));
        sampleError("Can't set format.", DEFAULT_WINDOW_HEIGHT);

    if ((pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels)) < 0)
        //printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));
        sampleError("Can't set channels number.", DEFAULT_WINDOW_HEIGHT);

    if ((pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0)) < 0)
        //printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));
        sampleError("Can't set rate. ", DEFAULT_WINDOW_HEIGHT);

    /* Write parameters */
    if ((pcm = snd_pcm_hw_params(pcm_handle, params)) < 0)
        //printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));
        sampleError("Can't set hardware parameters. ", DEFAULT_WINDOW_HEIGHT);

    /* Resume information */
    //printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

    //printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

    snd_pcm_hw_params_get_channels(params, &tmp);
    //printf("channels: %i ", tmp);

    /* Mono/stereo check
    if (tmp == 1)
        printf("(mono)\n");
    else if (tmp == 2)
        printf("(stereo)\n");
    */

    snd_pcm_hw_params_get_rate(params, &tmp, 0);
    //printf("rate: %d bps\n", tmp);

    //printf("seconds: %.2f\n", seconds);

    /* Allocate buffer to hold single period */
    snd_pcm_hw_params_get_period_size(params, &frames, 0);

    buff_size = frames * channels * 2 /* 2 -> sample size */;
    buff = (char *) malloc(buff_size);

    snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

    for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) {

        if ((pcm = read(fd, buff, buff_size)) == 0) {
            return;
        }

        if ((pcm = snd_pcm_writei(pcm_handle, buff, frames)) == -EPIPE) {
            printf("XRUN.\n");
            snd_pcm_prepare(pcm_handle);
        } else if (pcm < 0) {
            printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
        }

    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buff);
}

/* Sample main function for testing playback
* Comment out when compiling against another main function
*int main(int argc, char **argv) {
*    int rate, channels, seconds, fd;
*    if (argc < 5) {
*        printf("Usage: %s <sample_rate> <channels> <seconds> <filename>\n",
*                                argv[0]);
*        return -1;
*    }
*
*    rate     = atoi(argv[1]);
*    channels = atoi(argv[2]);
*    seconds  = atoi(argv[3]);
*    fd = open(argv[4], O_RDONLY);
*    playback(rate, channels, seconds, fd);
*
*    return 0;
*}
*/
