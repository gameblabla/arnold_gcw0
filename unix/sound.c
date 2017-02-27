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

#include "../cpc/cpcglob.h"
#include "alsasound-common.h"
#include "osssound.h"
#include "pulseaudiosound.h"

#define SOUND_PLUGIN_NONE 0
#define SOUND_PLUGIN_OSS 1
#define SOUND_PLUGIN_ALSA 2
#define SOUND_PLUGIN_ALSA_MMAP 3
#define SOUND_PLUGIN_SDL 4
#define SOUND_PLUGIN_PULSE 5
#define SOUND_PLUGIN_AUTO 6

extern int sound_plugin;

char *soundpluginNames[] = {"NONE", "OSS", "ALSA", "ALSAMMAP", "SDL", "PULSE", "AUTO"};
#define NR_SOUND_PLUGINS 7

void convert8to16bit(signed short *ptr, long cptr) {
	signed short *dest;
	unsigned char *src;
	long srcbytes;
	//fprintf(stderr,"convert8to16bit ptr: %i, cptr:%i\n", ptr, cptr);
	srcbytes = cptr*4;
	dest = ptr + (srcbytes-2);
	src = (unsigned char *) (ptr + (srcbytes/2-1));
	//src = ptr + (srcbytes/2-1);
	while(srcbytes-- > 0) {
		//fprintf(stderr,"convert8to16bit dest: %i, src:%i, d_dest: %i, d_src: %i\n", dest, src, (((long)dest)-((long)ptr)), (((long)src)-((long)ptr)));
		*dest-- = ((*src--)-128)<<8;
	}
}

int getSoundplugin(char *s) {
	int i;
	for (i=0; i<NR_SOUND_PLUGINS; i++) {
		if (!strcmp(soundpluginNames[i],s)) {
			return i;
		}
	}
	return SOUND_PLUGIN_AUTO;
}

int autoDetectSoundplugin() {
#ifdef HAVE_PULSE
	if (pulseaudio_AudioPlaybackPossible()) {
		return SOUND_PLUGIN_PULSE;
	}
#endif
	if (oss_AudioPlaybackPossible()) {
		return SOUND_PLUGIN_OSS;
	}
#ifdef HAVE_ALSA
	if (alsa_AudioPlaybackPossible()) {
		return SOUND_PLUGIN_ALSA;
	}
#endif
	return SOUND_PLUGIN_NONE;
}

BOOL sound_throttle(void) {
	switch(sound_plugin) {
		case SOUND_PLUGIN_OSS:
			return oss_Throttle();
#ifdef HAVE_ALSA
		case SOUND_PLUGIN_ALSA:
			return alsa_Throttle();
		case SOUND_PLUGIN_ALSA_MMAP:
			return alsa_Throttle();
#endif
		case SOUND_PLUGIN_SDL:
			return FALSE;
#ifdef HAVE_PULSE
		case SOUND_PLUGIN_PULSE:
			return pulseaudio_Throttle();
#endif
		default:
			return FALSE;
	}
}

