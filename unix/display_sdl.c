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
/* display_sdl */
#ifdef HAVE_SDL
#include <SDL/SDL.h>
#include "display.h"
#include "sound.h"
#include <string.h>
#include <stdlib.h>
#include "../cpc/messages.h"

SDL_Surface *screen;
SDL_Surface *buf_sdl;
BOOL fullscreen = FALSE;	//FIXME
BOOL toggleFullscreenLater = FALSE;
int scale = 1;
//int mode = SDL_SWSURFACE;
#ifdef SDL_TRIPLEBUF
int mode = SDL_HWSURFACE|SDL_TRIPLEBUF;
#else
int mode = SDL_HWSURFACE;
#endif

static INLINE void debug(char *s) {
	fprintf(stderr,"%s\n",s);
}

void sdl_InitialiseKeyboardMapping(int);
void sdl_InitialiseJoysticks(void);

#define CPC_WARPFACTORMAX 16 /* Maximum allowed warp speed factor */
int cpc_warpfactor=1;        /* Current CPC warp speed factor */
int sdl_warpfacdisptime;    /* Number of screen updates left until the warp factor is no longer displayed */

void sdl_SetDisplay(int Width, int Height, int Depth, BOOL wantfullscreen) {

	int i,accepted,format;

	fullscreen = wantfullscreen;
	fprintf(stderr, Messages[106], Width, Height, Depth);
	if ( !fullscreen ) {
		mode &= ~SDL_OPENGL;
		mode &= ~SDL_FULLSCREEN;
	} else {
		/* Fullscreen requested */
		mode |= SDL_FULLSCREEN;
	}
	screen = SDL_SetVideoMode(320, 240, Depth, mode);
	buf_sdl = SDL_CreateRGBSurface(SDL_HWSURFACE, 320+32, Height, 32, 0,0,0,0);
	
	if ( screen == NULL ) {
		fprintf(stderr, Messages[107],
			Width, Height, Depth, SDL_GetError());
		exit(1);
	}
}

void sdl_SetDisplayWindowed(int Width, int Height, int Depth) {

	sdl_SetDisplay(Width, Height, Depth, FALSE);
	SDL_ShowCursor(SDL_ENABLE);

}

void sdl_SetDisplayFullscreen(int Width, int Height, int Depth) {

	sdl_SetDisplay(Width, Height, Depth, TRUE);
	SDL_ShowCursor(SDL_DISABLE);

}

void sdl_toggleDisplayFullscreen() {
	fullscreen = !fullscreen;
	if (fullscreen) {
		sdl_SetDisplayFullscreen(screen->w,screen->h,
				screen->format->BitsPerPixel);
	} else {
		sdl_SetDisplayWindowed(screen->w,screen->h,
				screen->format->BitsPerPixel);
	}
}

void sdl_SetDoubled( BOOL doubled ) {

	int w, h;

	scale = doubled ? 2 : 1;
	if ( doubled ) {
		w = screen->w * 2;
		h = screen->h * 2;
	} else {
		w = screen->w / 2;
		h = screen->h / 2;
	}
	sdl_SetDisplay( w, h, screen->format->BitsPerPixel, fullscreen);

}

static INLINE unsigned long CalcBPPFromMask(unsigned long Mask)
{
	unsigned long LocalShift = 0;
	unsigned long LocalBPP = 0;
	unsigned long LocalMask = Mask;

	if (LocalMask!=0)
	{
		do
		{
			if ((LocalMask & 0x01)!=0)
				break;

			LocalMask = LocalMask >>1;
			LocalShift++;
		}
		while (1==1);

		do
		{
			if ((LocalMask & 0x01)!=1)
				break;

			LocalMask = LocalMask>>1;
			LocalBPP++;
		}
		while (1==1);
	}

	return LocalBPP;
}

void sdl_GetGraphicsBufferColourFormat(GRAPHICS_BUFFER_COLOUR_FORMAT *pFormat) {
	SDL_PixelFormat *format = screen->format;

	pFormat->Red.Mask = format->Rmask;
	pFormat->Red.BPP = CalcBPPFromMask(format->Rmask);
	pFormat->Red.Shift = format->Rshift;

	pFormat->Green.Mask = format->Gmask;
	pFormat->Green.BPP = CalcBPPFromMask(format->Gmask);
	pFormat->Green.Shift = format->Gshift;

	pFormat->Blue.Mask = format->Bmask;
	pFormat->Blue.BPP = CalcBPPFromMask(format->Bmask);
	pFormat->Blue.Shift = format->Bshift;

	pFormat->BPP = format->BitsPerPixel;

	printf("BPP: %d\r\n",pFormat->BPP);
	printf("Red: M: %08x B: %d S: %d\r\n",pFormat->Red.Mask, pFormat->Red.BPP, pFormat->Red.Shift);
	printf("Green: M: %08x B: %d S: %d\r\n", pFormat->Green.Mask, pFormat->Green.BPP, pFormat->Green.Shift);
	printf("Blue: M: %08x B: %d S: %d\r\n",pFormat->Blue.Mask, pFormat->Blue.BPP, pFormat->Blue.Shift);
}

int sdl_CheckDisplay(void) {
	return 0;
}


void sdl_GetGraphicsBufferInfo(GRAPHICS_BUFFER_INFO *pBufferInfo) {
	SDL_Surface *buffer;

	//buffer=screen;
	buffer = buf_sdl;
	pBufferInfo->pSurface = buffer->pixels;
	pBufferInfo->Width = buffer->w;
	pBufferInfo->Height = buffer->h;
	pBufferInfo->Pitch = buffer->pitch;
	//printf("get buffer info\r\n");
	//printf("W: %d H: %d P: %d\r\n",pBufferInfo->Width, pBufferInfo->Height,
		//pBufferInfo->Pitch);
}

BOOL sdl_LockGraphicsBuffer(void) {
	if (SDL_LockSurface(buf_sdl) == 0) return TRUE;
	else return FALSE;
}

void sdl_UnlockGraphicsBuffer(void) {
	SDL_UnlockSurface(buf_sdl);
}

/*
void scale32_array_fw_memcpy(Uint16 pitch, Uint32 *pixels32, int w, int h, int bpp) {
	int x, y, x2, y2;
	int ll = pitch/bpp;
	Uint32 *src, *dst, *dst2, pel, *srcstop;
	int i;

	y2 = h-1;
	for ( y = h/2-1; y >= 0; y-- ) {
		x2 = 0;
		for (x = 0; x < w/2; x ++) {
			pixels32[y2*ll+x2] = pixels32[y*ll+x];
			x2++;
			pixels32[y2*ll+x2] = pixels32[y*ll+x];
			x2++;
		}
		memcpy(pixels32+(y2-1)*ll, pixels32+y2*ll, pitch);
		y2-=2;
	}
}
*/

void sdl_DoubleGraphicsBuffer32(void) {
	/*int x, y;
	Uint16 pitch = screen->pitch;
	Uint32 *pixels32 = (Uint32 *) screen->pixels;
	int w = screen->w;
	int h = screen->h;
	int bpp = screen->format->BytesPerPixel;
	int ll = pitch/bpp;
	Uint32 *src, *dst, *dst2, pel, *srcstop;
	int i;

	SDL_LockSurface(screen);
	scale32_array_fw_memcpy(pitch, pixels32, w, h, bpp);
	SDL_UnlockSurface(screen);*/
}

void sdl_DoubleGraphicsBuffer16(void) {
	/*int x, y;
	Uint16 pitch = screen->pitch;
	Uint16 *pixels16 = (Uint16 *) screen->pixels;
	int w = screen->w;
	int h = screen->h;
	int bpp = screen->format->BytesPerPixel;
	int ll = pitch/bpp;
	Uint16 *src, *dst, *dst2, pel;
	int i;

	SDL_LockSurface(screen);
	for ( y = h-1; y >= 0; y-=2 ) {
		dst = pixels16+y*ll+w-1;
		dst2 = pixels16+(y-1)*ll+w-1;
		src = pixels16+y/2*ll+w/2-1;
		for ( x = w/2-1; x >= 0; x-- ) {
			pel = *src--;
			*dst-- = pel;
			*dst-- = pel;
			*dst2-- = pel;
			*dst2-- = pel;
		}
	}
	SDL_UnlockSurface(screen);*/
}

void sdl_DoubleGraphicsBuffer8(void) {
	/*int x, y;
	Uint16 pitch = screen->pitch;
	Uint8 *pixels8 = (Uint8 *) screen->pixels;
	int w = screen->w;
	int h = screen->h;
	int bpp = screen->format->BytesPerPixel;
	int ll = pitch/bpp;
	Uint8 *src, *dst, *dst2, pel;
	int i;

	SDL_LockSurface(screen);
	for ( y = h-1; y >= 0; y-=2 ) {
		dst = pixels8+y*ll+w-1;
		dst2 = pixels8+(y-1)*ll+w-1;
		src = pixels8+y/2*ll+w/2-1;
		for ( x = w/2-1; x >= 0; x-- ) {
			pel = *src--;
			*dst-- = pel;
			*dst-- = pel;
			*dst2-- = pel;
			*dst2-- = pel;
		}
	}
	SDL_UnlockSurface(screen);*/
}

void sdl_DoubleGraphicsBuffer(void) {

	int bpp = screen->format->BytesPerPixel;
	switch( bpp ) {
		case 1:
			sdl_DoubleGraphicsBuffer8();
			break;
		case 2:
			sdl_DoubleGraphicsBuffer16();
			break;
		case 4:
			sdl_DoubleGraphicsBuffer32();
			break;
		default:	/* unsupported */
			break;
	}
}

void sdl_SwapGraphicsBuffers(void) 
{
	int position_x, position_y;
	SDL_Rect pos;
	position_x = -33;
	position_y = -32;
	pos.x = position_x;
	pos.y = position_y;
	//if ( scale == 2 ) sdl_DoubleGraphicsBuffer();
	//SDL_UpdateRects(screen,1,&screen->clip_rect);
	SDL_BlitSurface(buf_sdl, NULL, screen, &pos);
	SDL_Flip( screen );
}

/* Some comments about throttling on Linux/Unix:
 * Originally I used the nice and portable SDL_GetTicks() and SDL_Delay()
 * functions. Unfortunately the resolution is bad and the result is very
 * dodgy. Therefore I used the gettimeofday() and usleep() functions. This is
 * POSIX, but not SDL and therefore not directly portable to non-POSIX SDL
 * Targets.
 * I left in the SDL in case someone wants to try.
 * FIXME: Maybe we can get rid of floating point here?
 */
/* Get elapsed time in microseconds
   Works correctly for time intervals up to ~4000 seconds */
unsigned long sdl_GetMicrosecondsDelta()
{
	static struct timeval t1, t2;
	unsigned long dt;

	gettimeofday(&t1,NULL);
	dt=(t1.tv_sec - t2.tv_sec)*1000000+(t1.tv_usec - t2.tv_usec); /* 1000000 microseconds in a second... */
	//printf("\ntime: %i %i %i %i delta: %f\n",t1.tv_sec, t2.tv_sec, t1.tv_usec, t2.tv_usec, dt);
	memcpy( &t2, &t1, sizeof(t2) );
	return dt;
}
/*
unsigned long long sdl_GetMicroseconds() {
	struct timeval t1;

	gettimeofday(&t1,NULL);
	return (t1.tv_sec*1000000+t1.tv_usec);
}
*/

#define SDL_REFRESHMAX 200

/* Determine current screen refresh rate
   This only works if the video driver has sync to vblank enabled.
   Returns 0 if unsuccessful. */
int sdl_RefreshRate(void) {
	int i,refresh;
	int refreshguess[SDL_REFRESHMAX];

	/*memset(refreshguess,0,sizeof(refreshguess));
	sdl_GetMicrosecondsDelta();
	for (i=0;i<70;i++) {
		SDL_Flip( screen );
		refresh=1e6/sdl_GetMicrosecondsDelta();
		//printf("Got refresh rate %d\n",refresh);
		if (refresh<SDL_REFRESHMAX) {
			refreshguess[refresh]++;
			if (refreshguess[refresh]==10) {
				if (refresh<45) {
					return(0);
				}
				return(refresh);
			}
		}
	}*/
	
	/* No luck, inconsistent results */
	return(0);
}

int sdl_fps = 50;

/* Set SDL frames per second */
void sdl_SetFPS(int fps) {
	sdl_fps=fps;
}
int sdl_LockSpeed = TRUE;
//#ifdef BUSYWAIT
//static unsigned long PreviousTime=0;
//unsigned long TimeError = 0;
//#endif

void sdl_Throttle(void) {
	/*if (toggleFullscreenLater) {
		toggleFullscreenLater = FALSE;
		sdl_toggleDisplayFullscreen();
	}*/
	/*if (sdl_LockSpeed)
	{
		static Uint32 next_tick = 0;
		Uint32 this_tick;

		if (!sound_throttle()) {
			this_tick = SDL_GetTicks();
			if ( this_tick < next_tick ) {
				SDL_Delay(next_tick-this_tick);
				next_tick = next_tick + (1000/sdl_fps);
			} else {
				next_tick = this_tick + (1000/sdl_fps);
			}
		}
		//fprintf(stderr,"(%i %i) ",this_tick,next_tick);
	}*/

	CPC_UpdateAudio();
	sdl_HandleMouse(NULL);
}

#include "keyboard_sdl.c"
#endif		/* HAVE_SDL */

