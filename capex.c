/*  CAPEX for FBA2X

	Copyright (C) 2007  JyCet
	Copyright (C) 2008 Juanvvc. Adapted from capex for cps2emu by Jycet

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

// CAPEX
// FBA2X Frontend
// JYCET & Juanvvc 2K8
// v0.1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL/SDL.h>

#include <fcntl.h>


#include "./gfx/gfx_BG.h"
#include "./gfx/gfx_CAPEX.h"
#include "./gfx/gfx_FONT.h"
#include "./gfx/gfx_SELECTEUR.h"

#include "capex.h"

#ifdef WIN32
#define sync()
#undef main
#endif

extern char **environ;

int KEYLEFT,KEYRIGHT,KEYUP,KEYDOWN,KEYA,KEYB,KEYX,KEYY,KEYSTART,KEYSELECT,KEYLS,KEYRS,KEYQ,KEYPAUSE;
int KeysConfigured= 0;

SDL_Event event;

SDL_Surface *screen2;
SDL_Surface *screen;
SDL_Surface *bg;
SDL_Surface *bgs;
SDL_Surface *bg_temp;
SDL_Surface *font;
SDL_Surface *barre;
SDL_Surface *preview;
SDL_Surface *title;
SDL_Surface *help;
SDL_Surface *credit;
SDL_Surface *Tmp;

SDL_RWops *rw;

FILE *fp;
FILE *fp2;
//FILE *fp3;

unsigned int font6x[255];
int i;
unsigned int ii;
unsigned char flag_preview;
char g_string[255];
char ar;
char * path;
char * argument[N_ARG];
//unsigned char title_y;
unsigned char flag_save;
unsigned char flag_TV;
unsigned char offset_x , offset_y ;
Uint32 fps_count;
//char *cf;

struct data
{
	unsigned int nb_rom;
	unsigned int nb_cache;
	unsigned int nb_list[NB_FILTRE];
	char *name[NB_MAX_GAMES];
	char *zip[NB_MAX_GAMES];
	char *status[NB_MAX_GAMES];
	char *parent[NB_MAX_GAMES];
	unsigned char etat[NB_MAX_GAMES];
	unsigned int longueur[NB_MAX_GAMES];
	unsigned int long_max;
} data;

unsigned int listing_tri[NB_FILTRE][NB_MAX_GAMES];

struct options
{
	unsigned char y;
	unsigned char num;
	unsigned int offset_num;

	unsigned int cpu;
	unsigned char sound;
	unsigned int samplerate;
	unsigned char rescale;
	unsigned char rotate;
	unsigned char showfps;
	unsigned char frontend;
	unsigned char m68kcore;

	unsigned char nb;
	unsigned char listing;
	unsigned char z80core;
	unsigned char filter;
	unsigned char spare1;
	unsigned char spare2;
	unsigned char spare3;
	unsigned char spare4;
	unsigned char spare5;
	unsigned int sense;


} options;

struct conf
{
	unsigned char exist;
	char *cf;
	unsigned int cpu;
	unsigned char sound;
	unsigned int samplerate;
	unsigned char rescale;
	unsigned char rotate;
	unsigned char showfps;
	unsigned char m68kcore;
	unsigned char z80core;
	unsigned char filter;
	unsigned char spare1;
	unsigned char spare2;
	unsigned char spare3;
	unsigned char spare4;
	unsigned char spare5;
	unsigned int sense;

}conf;

struct capex
{
	unsigned char clock;
	unsigned char tweak;
	unsigned char delayspeed;
	signed char repeatspeed;
	unsigned char list;
	signed char FXshadow;
	float FLshadow;
	unsigned char skin;
	signed char deadzone;
	char *rompath[1000];
} capex;

struct selector
{
	unsigned char y;
	unsigned char crt_x;
	unsigned int num;
	unsigned int offset_num;
} selector;

struct run
{
	unsigned char y;
	signed char num;
}run;

#include "capex_ecriture.h"
#include "capex_lecture.h"
#include "capex_tri.h"
#include "capex_pixel.h"

void redraw_screen(void)
{
	SDL_Rect srcRect;
	SDL_Rect dstRect;

	SDL_Delay(16);
	if (flag_TV){
		drawSprite(screen, screen2, 0, 0, offset_x, offset_y, 320, 240);
		SDL_UpdateRect( screen2 , offset_x, offset_y, 320, 240 );
		while ( ((SDL_GetTicks() - fps_count) <<10 ) < 17066 ); //17066 ~ 60Hz | 20480 ~ 50Hz
		fps_count = SDL_GetTicks();
	} else {
		SDL_BlitSurface(screen, NULL, screen2, NULL);
		SDL_Flip(screen2);
	}
}

void free_memory(void)
{
	printf("Freeing surfaces\n");
	if (screen2) SDL_FreeSurface(screen2);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(bg);
	SDL_FreeSurface(bgs);
	SDL_FreeSurface(bg_temp);
	SDL_FreeSurface(font);
	SDL_FreeSurface(barre);
	SDL_FreeSurface(preview);
	SDL_FreeSurface(title);
	SDL_FreeSurface(help);
	SDL_FreeSurface(credit);
	printf("Freeing memory\n");
	for (ii=0;ii<data.nb_list[0];++ii){
		free(data.name[ii]);
		free(data.zip[ii]);
		free(data.status[ii]);
		free(data.parent[ii]);
	}
}

int exit_prog(void)
{
	//menage avant execution
	free_memory();

	SDL_JoystickClose(0);
	SDL_Quit();

	return 0;

}

void put_string(char *string, unsigned int pos_x, unsigned int pos_y, unsigned char couleur, SDL_Surface *s)
{
	SDL_Rect Src;
	SDL_Rect Dest;

	Src.y = couleur;
	Src.w = 5;
	Src.h = 9;
	Dest.y = pos_y;

	while(*string)
	{
		if (font6x[*string]){
			Src.x = font6x[*string];
			Dest.x = pos_x;
			SDL_BlitSurface(font, &Src, s, &Dest);
		}
		++string;
		pos_x += 6;
	}
}

void put_stringM(char *string, unsigned int pos_x, unsigned int pos_y, unsigned int taille, unsigned char couleur)
{
	SDL_Rect Src;
	SDL_Rect Dest;
	unsigned char caratere;

	if( taille > selector.crt_x ){

		string += selector.crt_x ;

		Src.y = couleur;
		Src.w = 5;
		Src.h = 9;
		Dest.y = pos_y;


		if ( (taille-selector.crt_x) > 52 ) {
			for( caratere=selector.crt_x ; caratere<(selector.crt_x+48) ; ++caratere)
			{
				if (font6x[*string]){
					Src.x = font6x[*string];
					Dest.x = pos_x;
					SDL_BlitSurface(font, &Src, screen, &Dest);
				}
				++string;
				pos_x += 6;
			}
			for( caratere=0 ; caratere<3 ; ++caratere)
			{
				if (font6x[*string]){
					Src.x = font6x[46];
					Dest.x = pos_x;
					SDL_BlitSurface(font, &Src, screen, &Dest);
				}
				pos_x += 6;
			}
		}else{
			while(*string)
			{
				if (font6x[*string]){
					Src.x = font6x[*string];
					Dest.x = pos_x;
					SDL_BlitSurface(font, &Src, screen, &Dest);
				}
				++string;
				pos_x += 6;
			}
		}
	}
}

void show_screen(char * text)
{
	#define CREDIT_X	68
	#define CREDIT_Y	62
	#define CREDIT_L	184
	#define CREDIT_H	116
	Uint32 Pnoir = SDL_MapRGB(credit->format, 0, 0, 0);
	Uint32 Pblanc = SDL_MapRGB(credit->format, 255, 255, 255 );
	//capture screen actuel
	drawSprite( screen , credit , 0 , 0 , 0 , 0 , 320 , 240 );
	//detourage
	ligneV( credit , CREDIT_X-1 , CREDIT_Y , CREDIT_H , Pnoir );
	ligneV( credit , CREDIT_X+CREDIT_L , CREDIT_Y , CREDIT_H , Pnoir );
	ligneH( credit , CREDIT_X , CREDIT_Y-1 , CREDIT_L , Pnoir);
	ligneH( credit , CREDIT_X , CREDIT_Y+CREDIT_H , CREDIT_L , Pnoir);
	//cadre
	carre_plein( credit , CREDIT_X, CREDIT_Y, CREDIT_L, CREDIT_H, Pblanc);
	carre_plein( credit , CREDIT_X+1, CREDIT_Y+1, CREDIT_L-2, CREDIT_H-2, Pnoir);

	put_string( "Set Keys" , 142 , CREDIT_Y+8 , BLANC , credit );
	put_string( text , CREDIT_X+8 , CREDIT_Y+24 , BLEU , credit );
}

char set_keys()
{
	int not_pressed =1;
	unsigned int compteur = 1;


	show_screen("Press key for Up");

	while(not_pressed)
	{
		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYUP = event.key.keysym.sym; 
			  not_pressed =0;
                          			
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Down");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYDOWN = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Left");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYLEFT = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Right");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYRIGHT = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Fire 1");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYA = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Fire 2");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYB = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Fire 3");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYX = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Fire 4");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYY = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Fire 5");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYLS = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Fire 6");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYRS = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Start");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYSTART = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Coin");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYSELECT = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Quit");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYQ = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	not_pressed =1;
	show_screen("Press key for Pause");
	while(not_pressed)
	{

		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			  KEYPAUSE = event.key.keysym.sym; 
			  not_pressed =0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
	KeysConfigured = 1;

	Uint32 Pnoir = SDL_MapRGB(credit->format, 0, 0, 0);
	Uint32 Pblanc = SDL_MapRGB(credit->format, 255, 255, 255 );
	//capture screen actuel
	drawSprite( screen , credit , 0 , 0 , 0 , 0 , 320 , 240 );
	//detourage
	ligneV( credit , CREDIT_X-1 , CREDIT_Y , CREDIT_H , Pnoir );
	ligneV( credit , CREDIT_X+CREDIT_L , CREDIT_Y , CREDIT_H , Pnoir );
	ligneH( credit , CREDIT_X , CREDIT_Y-1 , CREDIT_L , Pnoir);
	ligneH( credit , CREDIT_X , CREDIT_Y+CREDIT_H , CREDIT_L , Pnoir);
	//cadre
	carre_plein( credit , CREDIT_X, CREDIT_Y, CREDIT_L, CREDIT_H, Pblanc);
	carre_plein( credit , CREDIT_X+1, CREDIT_Y+1, CREDIT_L-2, CREDIT_H-2, Pnoir);

	put_string( "Set Keys" , 142 , CREDIT_Y+8 , BLANC , credit );
	put_string( "Keys have been set.", CREDIT_X+8 , CREDIT_Y+24 , BLEU , credit );
        put_string( "Choose an option" , CREDIT_X+8 , CREDIT_Y+34 , BLEU , credit );
	put_string( "C) save to current rom keys." , CREDIT_X+8 , CREDIT_Y+44 , BLEU , credit );
	put_string( "G) save to global key. " , CREDIT_X+8 , CREDIT_Y+54 , BLEU , credit );
	put_string( "Hit any other key to discard." , CREDIT_X+8 , CREDIT_Y+64 , BLEU , credit );
	while(1)
	{
		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
				if ( event.key.keysym.sym==SDLK_c ){
					write_rom_key();
					return 0;
				} else if ( event.key.keysym.sym==SDLK_g ){
					write_global_key();
				return 0;
				} else return 0;
			}
			++compteur;
		} else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}

	return 0;
}

char ss_prg_credit(void)
{
	#define CREDIT_X	68
	#define CREDIT_Y	62
	#define CREDIT_L	184
	#define CREDIT_H	116

	unsigned int compteur = 1;
	Uint32 Pnoir = SDL_MapRGB(credit->format, 0, 0, 0);
	Uint32 Pblanc = SDL_MapRGB(credit->format, 255, 255, 255 );

	//capture screen actuel
	drawSprite( screen , credit , 0 , 0 , 0 , 0 , 320 , 240 );
	//detourage
	ligneV( credit , CREDIT_X-1 , CREDIT_Y , CREDIT_H , Pnoir );
	ligneV( credit , CREDIT_X+CREDIT_L , CREDIT_Y , CREDIT_H , Pnoir );
	ligneH( credit , CREDIT_X , CREDIT_Y-1 , CREDIT_L , Pnoir);
	ligneH( credit , CREDIT_X , CREDIT_Y+CREDIT_H , CREDIT_L , Pnoir);
	//cadre
	carre_plein( credit , CREDIT_X, CREDIT_Y, CREDIT_L, CREDIT_H, Pblanc);
	carre_plein( credit , CREDIT_X+1, CREDIT_Y+1, CREDIT_L-2, CREDIT_H-2, Pnoir);

	put_string( "CREDIT" , 142 , CREDIT_Y+8 , BLANC , credit );
	put_string( "CAPEX v0.7 by .... Juanvvc" , CREDIT_X+8 , CREDIT_Y+24 , BLEU , credit );
	put_string( "for FBA Emu by .... Juanvvc" , CREDIT_X+8 , CREDIT_Y+34 , BLEU , credit );
	put_string( "Code & design by ..... JyCet" , CREDIT_X+8 , CREDIT_Y+44 , VERT , credit );
	put_string( "Preview pack by ... Juanvvc" , CREDIT_X+8 , CREDIT_Y+54 , VERT , credit );
	put_string( "External skin by ... Pedator" , CREDIT_X+8 , CREDIT_Y+64 , VERT , credit );
	//put_string( "Betatest by ... Yod4z and me" , CREDIT_X+8 , CREDIT_Y+84 , VERT , credit );
	put_string( "Press (SELECT) to quit CAPEX" , 79 , CREDIT_Y+100 , BLANC , credit );

	while(1)
	{
		drawSprite( credit , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
				if ( event.key.keysym.sym==SDLK_ESCAPE ){
					return 1 ;
				}else return 0;
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
}
void ss_prg_help(void)
{
	#define HELP_X	53
	#define HELP_Y	62
	#define HELP_L	214
	#define HELP_H	116

	unsigned int compteur = 1;
	Uint32 Pnoir = SDL_MapRGB(help->format, 0, 0, 0);
	Uint32 Pblanc = SDL_MapRGB(help->format, 255, 255, 255 );

	//capture screen actuel
	drawSprite( screen , help , 0 , 0 , 0 , 0 , 320 , 240 );
	//detourage
	ligneV( help , HELP_X-1 , HELP_Y , HELP_H , Pnoir );
	ligneV( help , HELP_X+HELP_L , HELP_Y , HELP_H , Pnoir );
	ligneH( help , HELP_X , HELP_Y-1 , HELP_L , Pnoir);
	ligneH( help , HELP_X , HELP_Y+HELP_H , HELP_L , Pnoir);
	//cadre
	carre_plein( help , HELP_X, HELP_Y, HELP_L, HELP_H, Pblanc);
	carre_plein( help , HELP_X+1, HELP_Y+1, HELP_L-2, HELP_H-2, Pnoir);

	put_string( "COLOR HELP" , 130 , HELP_Y+8 , BLANC , help );
	put_string( "RED      missing" , HELP_X+8 , HELP_Y+24 , ROUGE , help );
	put_string( "ORANGE   clone rom detected" , HELP_X+8 , HELP_Y+34 , ORANGE , help );
	put_string( "YELLOW   parent or clone detected" , HELP_X+8 , HELP_Y+44 , JAUNE , help );
	put_string( "         & parent detected" , HELP_X+8 , HELP_Y+54 , JAUNE , help );
	put_string( "GREEN    clone & parent & cache" , HELP_X+8 , HELP_Y+64 , VERT , help );
	put_string( "         detected" , HELP_X+8 , HELP_Y+74 , VERT , help );
	put_string( "BLUE     parent & cache detected" , HELP_X+8 , HELP_Y+84 , BLEU , help );
	put_string( "Any button to return" , 100 , HELP_Y+100 , BLANC , help );

	int Hquit = 0;
	while(!Hquit)
	{
		drawSprite( help , screen , 0 , 0 , 0 , 0 , 320 , 240 );
		redraw_screen();

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			//if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
				//if ( event.key.keysym.sym==SDLK_A ){
				if ( event.key.keysym.sym > 0)
					if (compteur==0) Hquit = 1 ;
				//}
			//}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}
}

void init_title(void)
{
	//load background interne ou skin
	rw = SDL_RWFromMem(gfx_BG,sizeof(gfx_BG)/sizeof(unsigned char));
	Tmp = SDL_LoadBMP_RW(rw,0);
	if (capex.skin){
		if ((fp = fopen( "./skin/capex_bg.bmp" , "r")) != NULL){
			Tmp = SDL_LoadBMP( "./skin/capex_bg.bmp" );
			fclose(fp);
		}
	}
	bg = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	//load selector interne ou skin
	rw = SDL_RWFromMem(gfx_SELECTEUR,sizeof(gfx_SELECTEUR)/sizeof(unsigned char));
	Tmp = SDL_LoadBMP_RW(rw,0);
	if (capex.skin){
		if ((fp = fopen( "./skin/capex_selector.bmp" , "r")) != NULL){
			Tmp = SDL_LoadBMP( "./skin/capex_selector.bmp" );
			fclose(fp);
		}
	}
	barre = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);
	SDL_SetColorKey(barre ,SDL_SRCCOLORKEY,SDL_MapRGB(barre ->format,255,0,255));

	//load title interne ou skin
	rw = SDL_RWFromMem(gfx_CAPEX,sizeof(gfx_CAPEX)/sizeof(unsigned char));
	Tmp = SDL_LoadBMP_RW(rw,0);
	if (capex.skin){
		if ((fp = fopen( "./skin/capex_title.bmp" , "r")) != NULL){
			Tmp = (SDL_Surface *)IMG_Load( "./skin/capex_title.bmp" );
			fclose(fp);
		}
	}
	title = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);
	SDL_SetColorKey(title ,SDL_SRCCOLORKEY,SDL_MapRGB(title ->format,255,0,255));

	rw = SDL_RWFromMem(gfx_FONT,sizeof(gfx_FONT)/sizeof(unsigned char));
	Tmp = SDL_LoadBMP_RW(rw,0);
	font = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);
	SDL_FreeRW (rw);
	SDL_SetColorKey(font,SDL_SRCCOLORKEY,SDL_MapRGB(font->format,255,0,255));

	bg_temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000);
	bgs = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000);
	help = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000);
	credit = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000);

	drawSprite( bg , bgs , 0 , 0 , 0 , 0 , 320 , 240 );
	if (capex.FXshadow < 100) pixel_gamma( bgs );

	preparation_fenetre( bgs , bg , 124 , 3 , 192 , 112 );
	preparation_fenetre( bgs , bg , 4 , 53 , 116 , 62 );

	drawSprite( title , bg, 0, 0, 0, 0, 123, 52);
}

void prep_bg_run(void)
{
	drawSprite( bg , bg_temp , 0 , 0 , 0 , 0 , 320 , 240 );
	preparation_fenetre( bgs , bg_temp , 4 , 119 , 312 , 37 );
	if ( flag_preview )	drawSprite(preview, bg_temp, 0, 0, 220-preview->w/2, 3, 192, 112);
	//if ( flag_preview )	drawSprite(preview, bg_temp, 0, 0, 204, 3, 192, 112);

	put_string( "FBA2X clock:" , 6 , 55 , BLANC , bg_temp);
	put_string( "ROM" , 6 , 65 , BLANC , bg_temp);
		switch(capex.list)
		{
			case 0:
				sprintf((char*)g_string, "Database: %d roms" , data.nb_list[0] );
				break;
			case 1:
				sprintf((char*)g_string, "Missing: %d roms" , data.nb_list[1] );
				break;
			case 2:
				sprintf((char*)g_string, "Available: %d roms" , data.nb_list[2] );
				break;
			case 3:
				sprintf((char*)g_string, "Playable: %d roms" , data.nb_list[3] );
				break;
		}
		put_string( g_string , 6 , 105 , BLANC , bg_temp );
}

void prep_bg_options(void)
{
	drawSprite( bg , bg_temp , 0 , 0 , 0 , 0 , 320 , 240 );
	preparation_fenetre( bgs , bg_temp , 4 , 119 , 260 , 118 );
	preparation_fenetre( bgs , bg_temp , 269 , 119 , 47 , 118 );
	if ( flag_preview )	drawSprite(preview, bg_temp, 0, 0, 220-preview->w/2, 3, 192, 112);
	//if ( flag_preview )	drawSprite(preview, bg_temp, 0, 0, 204, 3, 192, 112);

	put_string( "FBA2X clock:" , 6 , 55 , BLANC , bg_temp);
	put_string( "ROM" , 6 , 65 , BLANC , bg_temp);
		switch(capex.list)
		{
			case 0:
				sprintf((char*)g_string, "Database: %d roms" , data.nb_list[0] );
				break;
			case 1:
				sprintf((char*)g_string, "Missing: %d roms" , data.nb_list[1] );
				break;
			case 2:
				sprintf((char*)g_string, "Available: %d roms" , data.nb_list[2] );
				break;
			case 3:
				sprintf((char*)g_string, "Playable: %d roms" , data.nb_list[3] );
				break;
		}
		put_string( g_string , 6 , 105 , BLANC , bg_temp );
}

void prep_bg_list(void)
{
	drawSprite( bg , bg_temp , 0 , 0 , 0 , 0 , 320 , 240 );
	preparation_fenetre( bgs , bg_temp , 4 , 119 , 312 , 118 );
	if ( flag_preview )	drawSprite(preview, bg_temp, 0, 0, 220-preview->w/2, 3, 192, 112);
	//if ( flag_preview )	drawSprite(preview, bg_temp, 0, 0, 204, 3, 192, 112);

	put_string( "FBA2X clock:" , 6 , 55 , BLANC , bg_temp);
	put_string( "ROM" , 6 , 65 , BLANC , bg_temp);
		switch(capex.list)
		{
			case 0:
				sprintf((char*)g_string, "Database: %d roms" , data.nb_list[0] );
				break;
			case 1:
				sprintf((char*)g_string, "Missing: %d roms" , data.nb_list[1] );
				break;
			case 2:
				sprintf((char*)g_string, "Available: %d roms" , data.nb_list[2] );
				break;
			case 3:
				sprintf((char*)g_string, "Playable: %d roms" , data.nb_list[3] );
				break;
		}
		put_string( g_string , 6 , 105 , BLANC , bg_temp );
}

void affiche_BG(void)
{
		drawSprite( bg_temp , screen , 0 , 0 , 0 , 0 , 320 , 240 );

		sprintf((char*)g_string, "%d" , options.cpu );
		put_string( g_string , 96 , 55 , BLANC , screen );

		sprintf((char*)g_string, "%s.zip" , data.zip[listing_tri[capex.list][selector.num]]);
		put_string( g_string , 30 , 65 , BLANC , screen );

		if ( strcmp( data.parent[listing_tri[capex.list][selector.num]] , "fba" ) == 0 ){
			put_string( "Parent rom" , 6 , 75 , BLANC , screen );
		}else{
			sprintf((char*)g_string, "Clone of %s" , data.parent[listing_tri[capex.list][selector.num]]);
			put_string( g_string , 6 , 75 , BLANC , screen );
		}

		if (conf.exist){
			sprintf((char*)g_string, "%s.cf YES" , conf.cf);
			put_string( g_string , 6 , 85 , VERT , screen );
		}else{
			sprintf((char*)g_string, "%s.cf NO" , conf.cf);
			put_string( g_string , 6 , 85 , ROUGE , screen );
		}

		if ( data.status[listing_tri[capex.list][selector.num]] != NULL ){
			put_string( data.status[listing_tri[capex.list][selector.num]] , 6 , 95 , BLANC , screen );
		}

}

void affiche_ligne_options(unsigned char num, unsigned char y)
{
	#define OPTIONS_START_X	8
	#define CONF_START_X	272

	switch (num)
	{
		case OPTION_NUM_FBA2X_CPU:
			sprintf((char*)g_string, "FBA2X clock: %dMHz" , options.cpu );
			put_string( g_string , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				sprintf((char*)g_string, "%dMHz" , conf.cpu );
				put_string( g_string , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "---MHz" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_FBA2X_SOUND:
			if (options.sound) put_string( "Sound: Enable" , OPTIONS_START_X , y , BLANC , screen );
			else put_string( "Sound: Disable" , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				put_string( abreviation_cf[0][conf.sound] , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "-" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_FBA2X_SAMPLERATE:
			sprintf((char*)g_string, "Audio sample rate: %dHz" , options.samplerate );
			put_string( g_string , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				sprintf((char*)g_string, "%dHz" , conf.samplerate );
				put_string( g_string , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "-----Hz" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_FBA2X_RESCALE:
			if (options.rescale == 3) put_string( "Scale: Full" , OPTIONS_START_X , y , BLANC , screen );
			else
			if (options.rescale == 2) put_string( "Scale: Best Fit" , OPTIONS_START_X , y , BLANC , screen );
			else if (options.rescale == 1) put_string( "Scale: 2x" , OPTIONS_START_X , y , BLANC , screen );
			else put_string( "Scale: None" , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				put_string( abreviation_cf[1][conf.rescale] , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "-" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_FBA2X_SHOWFPS:
			if (options.showfps) put_string( "Show FPS: Enable" , OPTIONS_START_X , y , BLANC , screen );
			else put_string( "Show FPS: Disable" , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				put_string( abreviation_cf[0][conf.showfps] , CONF_START_X , y , VERT , screen );

			}else{
				put_string( "-" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_FBA2X_FRONTEND:
			if (options.rotate == 0xFF) put_string( "Frameskip: Auto" , OPTIONS_START_X , y , BLANC , screen );
			else {
				sprintf((char*)g_string, "Frameskip: %d" , options.rotate );
				put_string( g_string , OPTIONS_START_X , y , BLANC , screen );
			}
			if (conf.exist){
				if(conf.rotate == 0xFF) sprintf((char*)g_string, "%s" , "Auto" );
				else sprintf((char*)g_string, "%d" , conf.rotate );
				put_string( (char*)g_string , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "-" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_FBA2X_CONFIGKEY:
			put_string( "Configure keys" , OPTIONS_START_X , y , BLANC , screen );

			break;
		case OPTION_NUM_FBA2X_68K:
			if (options.m68kcore == 0) put_string( "68K Emu Core: C68k" , OPTIONS_START_X , y , BLANC , screen );
			else
			if (options.m68kcore == 1) put_string( "68K Emu Core: M68K" , OPTIONS_START_X , y , BLANC , screen );
			//else put_string( "68K Emu Core: Auto" , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				put_string( abreviation_cf[3][conf.m68kcore] , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "-" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_CAPEX_DEADZONE:
			sprintf((char*)g_string, "Analogue Sensitivity: %d%%" , options.sense );
			put_string( g_string , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				sprintf((char*)g_string, "%d%%" , conf.sense );
				put_string( g_string , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "---%%" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_FBA2X_Z80:
			if (options.z80core == 0) put_string( "Z80 Emu Core: CZ80" , OPTIONS_START_X , y , BLANC , screen );
			else put_string( "Z80 Emu Core: MAME_Z80" , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				put_string( abreviation_cf[4][conf.z80core] , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "-" , CONF_START_X , y , ROUGE , screen );
			}
			break;
		case OPTION_NUM_CAPEX_TWEAK:
			if (options.filter == 1) put_string( "Use memory swap to file: On" , OPTIONS_START_X , y , BLANC , screen );
			else put_string( "Use memory swap to file: Off" , OPTIONS_START_X , y , BLANC , screen );
			if (conf.exist){
				put_string( abreviation_cf[5][conf.filter] , CONF_START_X , y , VERT , screen );
			}else{
				put_string( "-" , CONF_START_X , y , ROUGE , screen );
			}
			break;

		case OPTION_NUM_CAPEX_DELAYSPEED:
			sprintf((char*)g_string, "CAPEX delay speed: %d" , capex.delayspeed );
			put_string( g_string , OPTIONS_START_X , y , BLANC , screen );
			break;
		case OPTION_NUM_CAPEX_REPEATSPEED:
			sprintf((char*)g_string, "CAPEX repeat speed: %d" , capex.repeatspeed );

			put_string( g_string , OPTIONS_START_X , y , BLANC , screen );
			break;
		case OPTION_NUM_CAPEX_LIST:
			if (capex.list == 3) put_string( "Listing view: Playable only" , OPTIONS_START_X , y , BLANC , screen );
			else if (capex.list == 2) put_string( "Listing view: Available only" , OPTIONS_START_X , y , BLANC , screen );
			else if (capex.list == 1) put_string( "Listing view: Missing only" , OPTIONS_START_X , y , BLANC , screen );
			else put_string( "Listing view: All" , OPTIONS_START_X , y , BLANC , screen );
			break;
		case OPTION_NUM_CAPEX_SHADOW:
			sprintf((char*)g_string, "CAPEX background shadow: %d%c" , capex.FXshadow, 37 );
			put_string( g_string , OPTIONS_START_X , y , BLANC , screen );
			break;
		case OPTION_NUM_CAPEX_SKIN:
			if (capex.skin) put_string( "External CAPEX skin: Enable" , OPTIONS_START_X , y , BLANC , screen );
			else put_string( "External CAPEX skin: Disable" , OPTIONS_START_X , y , BLANC , screen );
			break;
		case OPTION_NUM_SAVE:
			put_string( "Save all settings" , OPTIONS_START_X , y , flag_save , screen );
			break;
		case OPTION_NUM_SAVE_CF:
			sprintf((char*)g_string, "Save FBA2X settings in %s.cf file" , conf.cf );
			put_string( g_string , OPTIONS_START_X , y , flag_save , screen );
			break;
		case OPTION_NUM_RETURN:
			put_string( "Return to the game list" , OPTIONS_START_X , y , BLANC , screen );
			break;
	}
}

void ss_prg_options(void)
{
	int Quit;
	unsigned int compteur = 1;
	unsigned char y;
	flag_save = VERT;
	options.y = START_Y-1;
	options.num = 0;
	options.offset_num = 0;

	unsigned option_start;

	prep_bg_options();

	Quit=0;
	while(!Quit)
	{
		affiche_BG();

		drawSprite(barre , screen, 0, 0, 4, options.y, 260, 10);
		drawSprite(barre , screen, 0, 0, 269, options.y, 47, 10);

		option_start = START_Y;
		for ( y = options.offset_num ; y<(options.offset_num+13) ; ++y){
			affiche_ligne_options( y , option_start );
			option_start += 9;
		}
		redraw_screen();
		//SDL_Flip(screen);

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
				if ( event.key.keysym.sym==SDLK_DOWN /*&& options.num<12 */){
					//++ options.num;
					//options.y += 9;
					if ( options.num == NOMBRE_OPTIONS && compteur==0 ){
						options.y = START_Y-1;
						options.num = 0;
						options.offset_num = 0;
					}else{
						if (options.num < 7 || options.offset_num == ( NOMBRE_OPTIONS - 12 ) ){
							if ( options.num < NOMBRE_OPTIONS ){
								options.y += 9;
								++options.num;
							}
						}else{
							++options.offset_num;
							++options.num;
						}
					}
				}else if ( event.key.keysym.sym==SDLK_UP /*&& options.num*/ ){
					//-- options.num;
					//options.y -= 9;
					if ( options.num == 0 && compteur==0 ){
						options.y = START_Y -1 + (12*9) ;
						options.num = NOMBRE_OPTIONS;
						options.offset_num = NOMBRE_OPTIONS - 12;
					}else{
						if ( options.num > ( NOMBRE_OPTIONS - 7 ) || options.offset_num == 0 ){
							if (options.num>0){
								options.y -= 9;
								--options.num;
							}
						}else{
							--options.offset_num;
							--options.num;
						}
					}
				}else if ( event.key.keysym.sym==SDLK_LEFT ){
					switch(options.num){
						case OPTION_NUM_CAPEX_DEADZONE:
							flag_save = ROUGE;
							--options.sense;
							if (options.sense < 10) options.sense=100;
							break;

						case OPTION_NUM_FBA2X_CPU:
							flag_save = ROUGE;
							break;
						case OPTION_NUM_FBA2X_Z80:
							flag_save = ROUGE;
							break;
						case OPTION_NUM_CAPEX_DELAYSPEED:
							flag_save = ROUGE;
							-- capex.delayspeed;
							if ( capex.delayspeed == 9 ) capex.delayspeed = 50;
							break;
						case OPTION_NUM_CAPEX_REPEATSPEED:
							flag_save = ROUGE;
							-- capex.repeatspeed;
							if ( capex.repeatspeed == -1 ) capex.repeatspeed = 3;
							break;
						case OPTION_NUM_CAPEX_SHADOW:
							flag_save = ROUGE;
							-- capex.FXshadow;
							if ( capex.FXshadow == -1 ) capex.FXshadow = 100;
							break;
					}
                } else if ( event.key.keysym.sym==SDLK_RIGHT ){
					switch(options.num){
						case OPTION_NUM_CAPEX_DEADZONE:
							flag_save = ROUGE;
							++options.sense;
							if (options.sense > 100) options.sense=10;
							break;

						case OPTION_NUM_FBA2X_CPU:
							flag_save = ROUGE;
							break;
						case OPTION_NUM_CAPEX_DELAYSPEED:
							flag_save = ROUGE;
							++ capex.delayspeed;
							if ( capex.delayspeed == 51 ) capex.delayspeed = 10;
							break;
						case OPTION_NUM_CAPEX_REPEATSPEED:
							flag_save = ROUGE;
							++ capex.repeatspeed;
							if ( capex.repeatspeed == 4 ) capex.repeatspeed = 0;
							break;
						case OPTION_NUM_CAPEX_SHADOW:
							flag_save = ROUGE;
							++ capex.FXshadow;
							if ( capex.FXshadow == 101 ) capex.FXshadow = 0;
							break;
					}
                    } else if ( event.key.keysym.sym==SDLK_LCTRL){
					switch(options.num){
						case OPTION_NUM_FBA2X_SOUND:
							flag_save = ROUGE;
							++options.sound;
							options.sound &= 1;
							break;
						case OPTION_NUM_FBA2X_SAMPLERATE:
							flag_save = ROUGE;
							options.samplerate = options.samplerate * 2;
							if ( options.samplerate == 88200 ) options.samplerate = 11025;
							break;
						case OPTION_NUM_FBA2X_RESCALE:
							flag_save = ROUGE;
							++options.rescale;
							if ( options.rescale > 3 ) options.rescale = 0;
							break;
						case OPTION_NUM_FBA2X_SHOWFPS:
							flag_save = ROUGE;
							++options.showfps;
							options.showfps &= 1;
							break;
						case OPTION_NUM_FBA2X_FRONTEND:
							flag_save = ROUGE;
							++options.rotate;
							if ( options.rotate > 59 ) options.rotate = 0xFF;
							break;
						case OPTION_NUM_FBA2X_68K:
							flag_save = ROUGE;
							++options.m68kcore;
							if (options.m68kcore>1) options.m68kcore= 0;
							break;
                        case OPTION_NUM_FBA2X_Z80:
                            flag_save = ROUGE;
							++options.z80core;
							if (options.z80core>1) options.z80core= 0;
							break;
						case OPTION_NUM_CAPEX_TWEAK:
							flag_save = ROUGE;
							++options.filter;
							if (options.filter>1) options.filter= 0;
							break;
						case OPTION_NUM_FBA2X_CONFIGKEY:
							flag_save = ROUGE;
							set_keys();
							break;
						case OPTION_NUM_CAPEX_LIST:
							flag_save = ROUGE;
							++capex.list;
							if ( capex.list == NB_FILTRE ) capex.list = 0;
							selector.y = START_Y-1;
							selector.crt_x=0;
							selector.num = 0;
							selector.offset_num = 0;
							break;
						case OPTION_NUM_CAPEX_SKIN:
							flag_save = ROUGE;
							++capex.skin;
							capex.skin &= 1;
							break;
						case OPTION_NUM_SAVE:
							write_cfg();
							if (KeysConfigured ) /*{
								write_global_key();
								KeysConfigured = 0;
							}*/
							flag_save = VERT;
							break;
						case OPTION_NUM_SAVE_CF:
							write_cf( );
							/*if (KeysConfigured ) {
								write_rom_key();
								KeysConfigured = 0;
							}*/
							flag_save = VERT;
							load_cf();
							load_cfg();
							break;
						case OPTION_NUM_RETURN:
							prep_bg_list();
							Quit = 1;
							break;
					}
				} else if(event.key.keysym.sym==SDLK_ESCAPE || event.key.keysym.sym == SDLK_LALT){
					prep_bg_list();
					Quit = 1;
				}
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}

	}
}

void ss_prog_run(void)
{
	#define RUN_START_X	8

	int Quit;
	unsigned int compteur = 1;
	if (conf.exist)
	{
		run.y = START_Y-1;
		run.num = 0;
	}
	else
	{
		run.y = START_Y+8;
		run.num = 1;
	}


	prep_bg_run();

	Quit=0;
	while(!Quit)
	{
		affiche_BG();
		drawSprite(barre , screen, 0, 0, 4, run.y, 312, 10);

		sprintf((char*)g_string, "Launch %s.zip with global settings" , data.zip[listing_tri[capex.list][selector.num]] );
		put_string( g_string , RUN_START_X , START_Y+9 , BLANC , screen );

		sprintf((char*)g_string, "Launch %s.zip with %s.cf settings" , data.zip[listing_tri[capex.list][selector.num]] , conf.cf);
		put_string( g_string , RUN_START_X , START_Y , BLANC , screen );

		put_string( "Return to the game list" , RUN_START_X , START_Y+18 , BLANC , screen );

		put_string( "Quit CAPEX" , RUN_START_X , START_Y+27 , BLANC , screen );

		redraw_screen();
		//SDL_Flip(screen);

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
				if ( event.key.keysym.sym==SDLK_DOWN ){
					++ run.num;
					if ( run.num == 4){
						run.y = START_Y-1;
						run.num = 0;
					}else{
						run.y += 9;
					}
				}else if ( event.key.keysym.sym==SDLK_UP ){
					-- run.num;
					if ( run.num == -1){
						run.y = START_Y -1 + (3*9);
						run.num = 3;
					}else{
						run.y -= 9;
					}
				}else if ( event.key.keysym.sym==SDLK_LCTRL ){
				    FILE * outlistno;
				    outlistno=fopen("./config/lastsel.cap","w");
				    fwrite(&selector.num,1,sizeof(int),outlistno);
				    fwrite(&selector.offset_num,1,sizeof(int),outlistno);
				    fwrite(&selector.y,1,sizeof(char),outlistno);
				    fclose(outlistno);
					switch (run.num)
					{
						case 1:

							ar=0;
							#ifdef WIN32
							argument[ ar ] = "fbasdl.exe";
							#else
							argument[ ar ] = "fbasdl.dge";
							#endif
							++ar;

							if(data.etat[listing_tri[capex.list][selector.num]] == BLEU)
								sprintf((char*)g_string, "%s/%s.fba" , capex.rompath,data.zip[listing_tri[capex.list][selector.num]]);
							else
								sprintf((char*)g_string, "%s/%s.zip" , capex.rompath,data.zip[listing_tri[capex.list][selector.num]]);

							argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
							strcpy(argument[ ar ],g_string);
							++ar;

							/*sprintf((char*)g_string, "--clock=%d" , options.cpu);
							argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
							strcpy(argument[ ar ],g_string);
							++ar;

							sprintf((char*)g_string, "--sense=%d" , options.sense);
							argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
							strcpy(argument[ ar ],g_string);
							++ar;*/

							if (options.sound){
								argument[ ar ] = "--sound-sdl";
								++ar;

								sprintf((char*)g_string, "--samplerate=%d" , options.samplerate);
								argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
								strcpy(argument[ ar ],g_string);
								++ar;
							}else{
								argument[ ar ] = "--no-sound";
								++ar;
							}

                           /* if (options.rescale == 3){
								//argument[ ar ] = "--hwho-rescale";
								argument[ ar ] = "--scaling=3";
								++ar;
							}else if (options.rescale == 2){
								argument[ ar ] = "--scaling=2";
								++ar;
							}else if (options.rescale == 1){
								argument[ ar ] = "--scaling=1";
								++ar;
							}else{
								argument[ ar ] = "--scaling=0";
								++ar;
							}*/

							if (options.rotate == 0xff){
								argument[ ar ] = "--frameskip=auto";
								++ar;
							}else{
								sprintf((char*)g_string, "--frameskip=%d" , options.rotate);
								argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
								strcpy(argument[ ar ], g_string);
								++ar;
							}

							if (options.m68kcore == 2){
								argument[ ar ] = "--force-c68k";
								++ar;
							}else if (options.m68kcore == 1){
								argument[ ar ] = "--force-m68k";
								++ar;
							}

							if (options.z80core == 1) {
								argument[ ar ] = "--z80core=1";
								++ar;
							}else if (options.z80core == 0){
								argument[ ar ] = "--z80core=0";
								++ar;
							}

							if (options.filter == 1){
								argument[ ar ] = "--use-swap";
								++ar;
							}

							if (options.showfps){
								argument[ ar ] = "--showfps";
								++ar	;
							}

							if (options.frontend){
								argument[ ar ] = "--frontend=./fbacapex.dge";
								++ar	;
							}

							argument[ ar ] = (char *)0;
							#ifdef WIN32
							path = "fbasdl.exe";
							#else
							path = "fbasdl.dge";
							#endif


							free_memory();
							SDL_Quit();

							execv(path, /*(const char * const *)*/argument);
							break;
						case 0:
							if (conf.exist) {
								//if (options.gamma != 100 ) set_gamma(options.gamma);

								ar=0;
								#ifdef WIN32
								argument[ ar ] = "fbasdl.exe";
								#else
								argument[ ar ] = "fbasdl.dge";
								#endif
								++ar;

								if(data.etat[listing_tri[capex.list][selector.num]] == BLEU)
									sprintf((char*)g_string, "%s/%s.fba" , capex.rompath,data.zip[listing_tri[capex.list][selector.num]]);
								else
									sprintf((char*)g_string, "%s/%s.zip" , capex.rompath,data.zip[listing_tri[capex.list][selector.num]]);

								argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
								strcpy(argument[ ar ],g_string);
								++ar;

								/*sprintf((char*)g_string, "--clock=%d" , conf.cpu);
								argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
								strcpy(argument[ ar ],g_string);
								++ar;

								sprintf((char*)g_string, "--sense=%d" , conf.sense);
								argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
								strcpy(argument[ ar ],g_string);
								++ar;*/

								if (conf.sound){
									argument[ ar ] = "--sound-sdl";
									++ar;

									sprintf((char*)g_string, "--samplerate=%d" , conf.samplerate);
									argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
									strcpy(argument[ ar ],g_string);
									++ar;
								}else{
									argument[ ar ] = "--no-sound";
									++ar;
								}

								/*if (conf.rescale == 3){
									//argument[ ar ] = "--hwho-rescale";
									argument[ ar ] = "--scaling=3";
									++ar;
								}else if (conf.rescale == 2){
									argument[ ar ] = "--scaling=2";
									++ar;
								}else if (conf.rescale == 1){
									argument[ ar ] = "--scaling=1";
									++ar;
								}else{
									argument[ ar ] = "--scaling=0";
									++ar;
								}*/

								if (conf.rotate == 0xff){
									argument[ ar ] = "--frameskip=auto";
									++ar;
								}else{
									sprintf((char*)g_string, "--frameskip=%d" , conf.rotate);
									argument[ ar ] = (char*) calloc( strlen(g_string) + 1 , sizeof(char));
									strcpy(argument[ ar ], g_string);
									++ar;
								}

								if (conf.m68kcore == 2){
									argument[ ar ] = "--force-c68k";
									++ar;
								}else if (conf.m68kcore == 1){
									argument[ ar ] = "--force-m68k";
									++ar;
								}

								if (conf.z80core == 1){
									argument[ ar ] = "--z80core=1";
									++ar;
								}else if (conf.z80core == 0){
									argument[ ar ] = "--z80core=0";
									++ar;
								}

								if (conf.filter == 1){
									argument[ ar ] = "--use-swap";
									++ar;
								}

								if (conf.showfps){
									argument[ ar ] = "--showfps";
									++ar	;
								}

								if (options.frontend){
									argument[ ar ] = "--frontend=./fbacapex.dge";
									++ar	;
								}

								argument[ ar ] = (char *)0;
								#ifdef WIN32
								path = "fbasdl.exe";
								#else
								path = "fbasdl.dge";
								#endif

								free_memory();
								SDL_Quit();

								execv(path, /*(const char * const *)*/argument);
							}
							break;
						case 2:
							prep_bg_list();
							Quit = 1;
							break;
						case 3:
							exit_prog();
							break;
					}
				} else if(event.key.keysym.sym==SDLK_LALT) {
					prep_bg_list();
					Quit = 1;
				}
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			compteur=0;// reinitialisation joystick
		}
	}

}

int findfirst(int l,int s)
{
    l-=32;
    //printf("letter to find %d\n",l);
    int z=0;
    while (z<data.nb_list[capex.list])
    {
        //printf("%s\n",data.name[listing_tri[capex.list][z]]);
        if (data.name[listing_tri[capex.list][z]][0]==l)
        {
            s=z;
            z=data.nb_list[capex.list];
        }
        z++;
    }
    //printf("go to line %d\n",s);
    if (s>data.nb_list[capex.list]-13) s=data.nb_list[capex.list]-13;
    return s;
}

int main(int argc, char *argv[])
{
	int Quit, ErrorQuit;
	unsigned int zipnum;
	unsigned int y;
	unsigned int compteur = 0;

	printf("CAPEX frontend for FBASDL\n");
	printf("v0.1 by JYCET, Opendingux port by exmortis@yandex.ru \n");

	SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_TIMER);


	screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000);
	screen2 = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE | SDL_DOUBLEBUF);

	flag_TV = 0;

	SDL_ShowCursor (0); //desactive souris
	SDL_JoystickOpen(0);

	load_cfg();
	init_title();


	put_string( "READING GAME LIST" , 130 , 100 ,BLANC, help );
	drawSprite(help,screen2,0,0,0,0,help->w,help->h);

	redraw_screen();

	if (lecture_zipname()) ErrorQuit = 1;
	else{
		tri_alphabeticAZ(0,data.nb_list[0]);
		if ( lecture_rominfo() ) ErrorQuit = 2;
		else{
			lecture_rom_jouable();
			ErrorQuit = 0;
		}
	}

	printf("Finished parsing roms\n");

	data.nb_list[1] = 0;
	data.nb_list[2] = 0;
	data.nb_list[3] = 0;
	for ( ii=0 ; ii<data.nb_list[0] ; ++ii){
		if (data.etat[listing_tri[0][ii]] == ROUGE ) {
			listing_tri[1][data.nb_list[1]] = listing_tri[0][ii];
			++data.nb_list[1];
		}else{
			listing_tri[2][data.nb_list[2]] = listing_tri[0][ii];
			++data.nb_list[2];

			if (data.etat[listing_tri[0][ii]] == VERT || data.etat[listing_tri[0][ii]] == BLEU ) {
				listing_tri[3][data.nb_list[3]] = listing_tri[0][ii];
				++data.nb_list[3];
			}
		}
	}

	printf("Finished searching roms\n");

	//precalcul font6 x coordonn�e
	for (i=0;i<32;++i) font6x[i]=0;
	for (i=32;i<255;++i) font6x[i]=(i-32)*6;

	selector.y = START_Y-1;
	selector.crt_x=0;
	selector.num = 0;
	selector.offset_num = 0;

	FILE * outlistno;
	outlistno=fopen("./config/lastsel.cap","r");
	if (outlistno)
	{
		fread(&selector.num,1,sizeof(int),outlistno);
		fread(&selector.offset_num,1,sizeof(int),outlistno);
		fread(&selector.y,1,sizeof(char),outlistno);
		printf("%u   %u   %c\n",selector.num,selector.offset_num,selector.y);
		fclose(outlistno);
		if (selector.num>data.nb_list[capex.list])
		{
			selector.y = START_Y-1;
			selector.crt_x=0;
			selector.num = 0;
			selector.offset_num = 0;
		}
		if (selector.num>data.nb_list[capex.list]-13)
		{
			selector.num=data.nb_list[capex.list]-13;
			selector.offset_num=selector.num;
			selector.y = START_Y-1;
		}
	}

	flag_preview = 0;
	load_preview(selector.num);
	load_cf();
	prep_bg_list();

	Quit = 0;
	while (!Quit)
	{
		affiche_BG();

		drawSprite(barre , screen, 0, 0, 4, selector.y, 312, 10);

		if (ErrorQuit == 1) put_string("erreur de lecture zipname.fba" , 8 , 220 , 0 , screen );
		else if (ErrorQuit == 2) put_string("erreur de lecture rominfo.fba" , 8 , 220 , 0 , screen );
		else{
			zipnum = START_Y;
			if ( data.nb_list[capex.list] < 14){
				for ( y = 0 ; y<data.nb_list[capex.list] ; ++y){
					put_stringM(data.name[listing_tri[capex.list][y]], // string
								8, // x
								zipnum, // y
								data.longueur[listing_tri[capex.list][y]], // length
								data.etat[listing_tri[capex.list][y]] ); // color
					zipnum += 9;
				}
			}else{
				for ( y = selector.offset_num ; y<(selector.offset_num+13) ; ++y){
					put_stringM(data.name[listing_tri[capex.list][y]], 
								8, 
								zipnum, 
								data.longueur[listing_tri[capex.list][y]], 
								data.etat[listing_tri[capex.list][y]] );
					zipnum += 9;
				}
			}
		}

		redraw_screen();
		//SDL_Flip(screen);

		SDL_PollEvent(&event);
		if (event.type==SDL_KEYDOWN){
			//printf("but: %d\r\n",event.key.keysym.sym);
			if (compteur==0 || (compteur>capex.delayspeed && ((compteur&joy_speed[capex.repeatspeed])==0))){
			    if ((event.key.keysym.sym>=SDLK_a) && (event.key.keysym.sym<=SDLK_z)){
			        selector.num=findfirst(event.key.keysym.sym,selector.num);
			        selector.offset_num=selector.num;
			        selector.y = START_Y-1;
			    }
				if (event.key.keysym.sym==SDLK_DOWN){
					// if in the end of list, reset to the beginning
					if ( selector.num == (data.nb_list[capex.list]-1) && compteur==0 ){
						selector.y = START_Y-1;
						selector.num = 0;
						selector.offset_num = 0;
					}else{
						if (data.nb_list[capex.list] < 14){ // if rom number in list < 14
								if (selector.num < (data.nb_list[capex.list]-1) ) {
									selector.y+=9;
									++selector.num;
									if (compteur==0){
										load_preview(selector.num);
										load_cf();
									}
								}
						}else{
							if (selector.num < 7 || selector.offset_num == (data.nb_list[capex.list]-13)) {
								if (selector.num < (data.nb_list[capex.list]-1)) {
									selector.y+=9;
									++selector.num;
									if (compteur==0){
										load_preview(selector.num);
										load_cf();
									}
								}
							} else {
								++selector.offset_num;
								++selector.num;
								if (compteur==0){
									load_preview(selector.num);
									load_cf();
								}
							}
						}
					}
				}else if (event.key.keysym.sym==SDLK_UP){
					if ( selector.num==0 && compteur==0 ){
						selector.num = data.nb_list[capex.list] - 1 ;
						if (data.nb_list[capex.list] < 14){
							selector.y = START_Y -1 + ( ( data.nb_list[capex.list] - 1 ) *9 ) ;
							//selector.offset_num = 0;
						}else{
							selector.y = START_Y -1 + (12*9) ;
							selector.offset_num = data.nb_list[capex.list] - 13;
						}
					}else{
						if ( selector.num > (data.nb_list[capex.list]-7) || selector.offset_num==0){
							if (selector.num>0){
								selector.y-=9;
								--selector.num;
								if (compteur==0){
									load_preview(selector.num);
									load_cf();
								}
							}
						}else{
							--selector.offset_num;
							--selector.num;
							if (compteur==0){
								load_preview(selector.num);
								load_cf();
							}
						}
					}
				}else if (event.key.keysym.sym==SDLK_LEFT && selector.crt_x>0){
					--selector.crt_x;
				}else if (event.key.keysym.sym==SDLK_RIGHT && selector.crt_x<data.long_max-53 ){
					++selector.crt_x;
				}else if (event.key.keysym.sym==SDLK_ESCAPE){
					if ( ss_prg_credit() ) Quit = 1;
				}else if (event.key.keysym.sym==SDLK_LCTRL){
					// executer l'emu
					if (data.etat[listing_tri[capex.list][selector.num]] != ROUGE) {
						ss_prog_run();
					}
				}else if( event.key.keysym.sym == SDLK_SPACE ){
					if (compteur==0) ss_prg_help();
				}else if( event.key.keysym.sym == SDLK_RETURN ){
					ss_prg_options();
				}
			}
			++compteur;
		}else if (event.type==SDL_KEYUP){
			if (compteur){
				load_preview(selector.num);
				load_cf();
			}
			compteur=0; // reinitialisation joystick
		}

	}

	exit_prog();
	return 0;
}


/*EOF*/
