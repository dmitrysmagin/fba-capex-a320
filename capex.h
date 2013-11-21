/*  
    CAPEX for FBASDL (OpenDingux)

    Copyright (C) 2012 OpenDingux adaptation from Fba2x-capex by d_smagin
    Copyright (C) 2007  JyCet

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define START_X	8
#define START_Y	120

#define NB_FILTRE	4
#define NB_MAX_GAMES	6000
#define N_ARG 50

#define BLANC	0
#define ROUGE	9
#define ORANGE	18
#define JAUNE	27
#define VERT	36
#define BLEU	45

#define OPTION_FBA_RUN					0
#define OPTION_FBA_SOUND				1
#define OPTION_FBA_SAMPLERATE			2
#define OPTION_FBA_RESCALE				3
#define OPTION_FBA_SHOWFPS				4
#define OPTION_FBA_68K					5
#define OPTION_FBA_Z80					6
#define OPTION_FBA_ANALOG				7
#define OPTION_FBA_SWAP					8
#define OPTION_FBA_FIRST				OPTION_FBA_RUN
#define OPTION_FBA_LAST					OPTION_FBA_SWAP

#define OPTION_NUM_CAPEX_DELAYSPEED		0
#define OPTION_NUM_CAPEX_REPEATSPEED	1
#define OPTION_NUM_CAPEX_LIST			2
#define OPTION_NUM_CAPEX_SHADOW			3
#define OPTION_NUM_CAPEX_SKIN			4
#define OPTION_NUM_CAPEX_DEADZONE		108
#define OPTION_NUM_RETURN				5

#define NOMBRE_OPTIONS				OPTION_NUM_RETURN

unsigned char joy_speed[4]={0,1,3,7};

static char *abreviation_cf[7][7]={
	{"Disable","Enable","","","","",""},
	{"Original","Fullscreen","","","","",""},
	{"Off","Auto","Manual","","","",""},
	{"C68k","M68k","A68k","","","",""},
	{"CZ80","MAME Z80","","","","",""},
	{"Off","On","","","","",""},
	{"Off","LIBAO","SDL","SDL old","","",""}
};

void ligneH(SDL_Surface* screenSurface, int x, int y, int w, Uint32 coul)
{
  SDL_Rect r;

  r.x = x;
  r.y = y;
  r.w = w;
  r.h = 1;

  SDL_FillRect(screenSurface, &r, coul);
}

void ligneV(SDL_Surface* screenSurface, int x, int y, int h, Uint32 coul)
{
  SDL_Rect r;

  r.x = x;
  r.y = y;
  r.w = 1;
  r.h = h;

  SDL_FillRect(screenSurface, &r, coul);
}

void carre_plein(SDL_Surface* screenSurface, int x, int y, int w, int h, Uint32 coul)
{
  SDL_Rect r;

  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;

  SDL_FillRect(screenSurface, &r, coul);
}

void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height)
{
	SDL_Rect srcRect;
	srcRect.x = srcX;
	srcRect.y = srcY;
	srcRect.w = width;
	srcRect.h = height;

	SDL_Rect dstRect;
	dstRect.x = dstX;
	dstRect.y = dstY;
	dstRect.w = width;
	dstRect.h = height;

	SDL_BlitSurface(imageSurface, &srcRect, screenSurface, &dstRect);
}

void preparation_fenetre(SDL_Surface *src, SDL_Surface *dest, int win_x , int win_y , int win_l , int win_h)
{
	Uint32 Pnoir = SDL_MapRGB(dest->format, 0, 0, 0);
	//preparation surface
	drawSprite( src , dest , win_x , win_y , win_x , win_y , win_l , win_h );
	ligneV( dest , win_x-1 , win_y , win_h , Pnoir );
	ligneV( dest , win_x+win_l , win_y , win_h , Pnoir );
	ligneH( dest , win_x , win_y-1 , win_l , Pnoir);
	ligneH( dest , win_x , win_y+win_h , win_l , Pnoir);
}
