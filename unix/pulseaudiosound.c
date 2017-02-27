/*
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifdef HAVE_PULSE

#include "pulseaudiosound.h"
#include "../cpc/host.h"
#include "display.h"
#include "gtkui.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include "../cpc/messages.h"
#include "sound.h"

#define TLENGTH (1760*4)

extern SOUND_PLAYBACK_FORMAT SoundFormat;	// FIXME

static const pa_sample_spec sampleSpec = {
//	.format = PA_SAMPLE_S16LE,
	.format = PA_SAMPLE_U8,
	.rate = 44100,
	.channels = 2
};

static const pa_buffer_attr bufferAttr = {
	.tlength = TLENGTH,
	.maxlength = (TLENGTH*3)/2,
	.minreq = TLENGTH/50,
	.prebuf = TLENGTH - TLENGTH/50,
	.fragsize = TLENGTH/50
};

pa_simple *s = NULL;
static signed short backbuffer[1760*2];
static int commitBufferSize;
static BOOL pulseaudio_audiodev_is_open = FALSE;

int error;

BOOL	pulseaudio_open_audio() {
	if (pulseaudio_audiodev_is_open) return TRUE;

	if (!(s = pa_simple_new(NULL, "Arnold", PA_STREAM_PLAYBACK, NULL, "playback", &sampleSpec, NULL, &bufferAttr, &error))) {
    	fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        return FALSE;
	}

	pulseaudio_audiodev_is_open = TRUE;

	return TRUE;
}

void	pulseaudio_close_audio(void) {
	if (!pulseaudio_audiodev_is_open) return;
	if (pa_simple_drain(s, &error) < 0) {
		fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
	}
	pa_simple_free(s);
	pulseaudio_audiodev_is_open = FALSE;
}

BOOL	pulseaudio_AudioPlaybackPossible(void)
{
	int err;
	err = pulseaudio_open_audio();
	fprintf(stderr,"pulseaudio_AudioPlaybackPossible(void): %i\n", err);
	return err;
}

SOUND_PLAYBACK_FORMAT *pulseaudio_GetSoundPlaybackFormat(void)
{
	SoundFormat.NumberOfChannels = sampleSpec.channels;
	SoundFormat.BitsPerSample = 8;
	/*if (format != SND_PCM_FORMAT_S8 && format != SND_PCM_FORMAT_U8)
		SoundFormat.BitsPerSample = 16;*/
	SoundFormat.Frequency = sampleSpec.rate;
	fprintf(stderr,"pulseaudio_GetSoundPlaybackFormat channels:%i, BitsPerSample: %i, Frequency: %i\n", SoundFormat.NumberOfChannels, SoundFormat.BitsPerSample, SoundFormat.Frequency);
	return &SoundFormat;
}

BOOL	pulseaudio_LockAudioBuffer(unsigned char **pBlock1, unsigned long
*pBlock1Size, unsigned char **pBlock2, unsigned long *pBlock2Size, int
AudioBufferSize)
{
	//printf("pulseaudio_LockAudioBuffer audiobuffersize: %i\n", AudioBufferSize);
	*pBlock1 = (unsigned char *) backbuffer;
	*pBlock1Size = AudioBufferSize;
	*pBlock2 = NULL;
	*pBlock2Size = 0;
	// in ALSA we measure this in samples, so we devide through channels
	// in pulseaudio we measure in bytes, so we do not device
	//commitBufferSize = AudioBufferSize/channels;
	commitBufferSize = AudioBufferSize;
	return TRUE;
}

void	pulseaudio_UnLockAudioBuffer(void)
{
	signed short *ptr;
	int err;
	long cptr;
	static int skipfirst = 1;

	//fprintf(stderr,"pulseaudio_UnLockAudioBuffer commitBufferSize: %i\n", commitBufferSize);
	// FIXME: Quickhack to get rid of click sound when playing
	// unitialized auio buffer on first entry.
	if (skipfirst) {
		skipfirst--;
		return;
	}
	if (!pulseaudio_audiodev_is_open) {
		return;
	}
	ptr = backbuffer;
	cptr = commitBufferSize;
	//if (format != SND_PCM_FORMAT_U8 && format != SND_PCM_FORMAT_S8) {
	//	convert8to16bit(ptr, cptr);
	//}
	while (cptr > 0) {
		if (pa_simple_write(s, ptr, (size_t) cptr, &error) < 0) {
			fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
			exit(1);
		}
		ptr += cptr;
		cptr -= cptr;
	}
}

BOOL	pulseaudio_Throttle(void)
{
	if (!pulseaudio_audiodev_is_open) return FALSE;
	return TRUE;
}

#endif	/* HAVE_PULSE */

