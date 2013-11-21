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

void put_string(char *string, unsigned int pos_x, unsigned int pos_y, unsigned char couleur, SDL_Surface *s);
void load_cfg()
{
	char arg1[128];
	char arg2[255];
	signed long argd;
	char ligne[256];

	// force default settings
	capex.tweak = 0;
	capex.delayspeed = 30;
	capex.repeatspeed = 1;
	capex.list = 0;
	capex.FXshadow = 90;
	capex.skin = 1;
        sprintf((char*)capex.rompath, "%s", "./roms/");

	// read config file
	if ((fp = fopen("./capex.cfg", "r")) != NULL){
		while(fgets(ligne,sizeof(ligne),fp) != NULL){

			sscanf(ligne, "%s %d", &arg1,&argd);
			sscanf(ligne, "%s %s", &arg1,&arg2);
			if (strcmp(arg1, "#") != 0) {
				if (strcmp(arg1,"capex_deadzone")==0) capex.deadzone = argd;
				else if (strcmp(arg1,"capex_tweak")==0) capex.tweak = argd;
				else if (strcmp(arg1,"capex_delay_speed")==0) capex.delayspeed = argd;
				else if (strcmp(arg1,"capex_repeat_speed")==0) capex.repeatspeed = argd;
				else if (strcmp(arg1,"capex_list")==0) capex.list = argd;
				else if (strcmp(arg1,"capex_shadow")==0) capex.FXshadow = argd;
				else if (strcmp(arg1,"capex_skin")==0) capex.skin = argd;
				else if (strcmp(arg1,"rom_path")==0) sprintf((char*)capex.rompath, "%s", arg2);
			}
		}
		fclose(fp);
	} else {
		write_cfg();
	}
}

void load_cf(void)
{
	char arg1[128];
	signed long argd;
	char ligne[256];


	if ( strcmp( data.parent[listing_tri[capex.list][selector.num]] , "fba" ) == 0 ){
		options.cf = (char*) calloc( strlen(data.zip[listing_tri[capex.list][selector.num]]) + 1 , sizeof(char));
		strcpy( options.cf , data.zip[listing_tri[capex.list][selector.num]] );
	}else{
		options.cf = (char*) calloc( strlen(data.parent[listing_tri[capex.list][selector.num]]) + 1 , sizeof(char));
		strcpy( options.cf , data.parent[listing_tri[capex.list][selector.num]] );
	}


	// read the specific config file
	sprintf((char*)g_string, "./config/%s.cf", options.cf);

	if ((fp = fopen( g_string , "r")) != NULL) {

		// mark file as present
		options.exist = 1;

		// force default settings
		options.sound = 2;
		options.samplerate = 22050;
		options.rescale = 0;
		options.frameskip = 0;
		options.showfps = 0;
		options.m68kcore = 0;
		options.z80core = 0;
		options.sense = 100;
		options.filter = 0;

		while(fgets(ligne,sizeof(ligne),fp) != NULL){

			sscanf(ligne, "%s %d", &arg1,&argd);

			if (strcmp(arg1,"#") != 0) {
				if (strcmp(arg1,"fba_sound")==0) options.sound = argd;
				else if (strcmp(arg1,"fba_samplerate")==0) options.samplerate = argd;
				else if (strcmp(arg1,"fba_rescale")==0) options.rescale = argd;
				else if (strcmp(arg1,"fba_frameskip")==0) options.frameskip = argd;
				else if (strcmp(arg1,"fba_sensitivity")==0) options.sense = argd;
				else if (strcmp(arg1,"fba_showfps")==0) options.showfps = argd;
				else if (strcmp(arg1,"m68k_core")==0) options.m68kcore = argd;
				else if (strcmp(arg1,"z80_core")==0) options.z80core = argd;
				else if (strcmp(arg1,"swap")==0) options.filter = argd;
			}
		}
		fclose(fp);
	} else {
		// mark file as absent
		options.exist = 0;

		// force default settings
		options.sound = 2;
		options.samplerate = 22050;
		options.rescale = 0;
		options.frameskip = 0;
		options.showfps = 0;
		options.m68kcore = 0;
		options.z80core = 0;
		options.sense = 100;
		options.filter = 0;
	}
}

char lecture_zipname(void)
{
	unsigned int caractere;
	unsigned int temp;
	unsigned char flag_v0,flag_v1;

	char ligne[256];
	char *arg;
	data.nb_list[0] = 0;
	data.long_max = 0;
	data.nb_rom = 0;

	printf("Reading zipname.fba\n");
	if ((fp = fopen("zipname.fba", "r")) != NULL){
		//printf("[");
		while(fgets(ligne,sizeof(ligne),fp) != NULL){

			//initialisation de la liste de tri�e
			listing_tri[0][data.nb_list[0]] = data.nb_list[0];

			flag_v0 = flag_v1 = 0;
			temp=strlen(ligne);

			for ( caractere = 0 ; caractere < temp ; ++caractere){
				if (strncmp( &ligne[caractere] ,"," , 1)==0){

					data.zip[data.nb_list[0]] = (char*) calloc( caractere + 1 , sizeof(char));
					strncpy(data.zip[data.nb_list[0]], ligne ,caractere);

					flag_v0 = caractere+1;
					break;
				}
			}

			for ( caractere = flag_v0 ; caractere < temp ; ++caractere){
				if (strncmp( &ligne[caractere] ,"," , 1)==0){
					flag_v1 = caractere;
					break;
				}
			}

			if (flag_v1){
					data.name[data.nb_list[0]] = (char*) calloc( (flag_v1-flag_v0) + 1 , sizeof(char));
					strncpy(data.name[data.nb_list[0]], &ligne[flag_v0] , (flag_v1-flag_v0) );

					data.status[data.nb_list[0]] = (char*) calloc( (temp  - flag_v1)  , sizeof(char));
					strncpy(data.status[data.nb_list[0]], &ligne[flag_v1+1] , (temp  - flag_v1 - 1) );
			}else{
					data.name[data.nb_list[0]] = (char*) calloc( (temp-flag_v0) , sizeof(char));
					strncpy(data.name[data.nb_list[0]], &ligne[flag_v0] , (temp-flag_v0)-1 );
			}

			data.longueur[data.nb_list[0]] = strlen(data.name[data.nb_list[0]]);
			if (data.long_max < data.longueur[data.nb_list[0]] ) data.long_max = data.longueur[data.nb_list[0]];

			//verification presence SET
			data.etat[data.nb_list[0]] = ROUGE ; //absent
			sprintf((char*)g_string, "%s/%s.zip", capex.rompath,data.zip[data.nb_list[0]] );
			if ((fp2 = fopen(g_string, "r")) != NULL){
			    fclose(fp2);
				data.etat[data.nb_list[0]] = ORANGE ;
				++data.nb_rom;
			}
			sprintf((char*)g_string, "./roms/%s.fba", data.zip[data.nb_list[0]] );
			if ((fp2 = fopen(g_string, "r")) != NULL){
				data.etat[data.nb_list[0]] +=9 ;
				++data.nb_cache;
			}

			//printf("-");

			//increment nombre de set detect�
			++data.nb_list[0];
		}fclose(fp);
		//printf("]\n");
	}else return 1;

	return 0;
}

void lecture_rom_jouable(void)
{
	unsigned flag_rom ;
	unsigned int num;
	int test=0;
	int test2=0;

	printf("debut analyse set %d\n",data.nb_list[0]);


	data.nb_cache = 0;

	//verification presence SET
	for ( num=0 ; num<data.nb_list[0] ; ++num ){

		//absent
		//data.etat[num] = ROUGE ;
		flag_rom = 0;

		if ( data.etat[num] == ORANGE ){
		    test2++;
		//test si rom parent
		if ( strcmp( data.parent[num] , "fba" ) == 0 ) {
			/*sprintf((char*)g_string, "./roms/%s.zip", data.zip[num] );
			printf("%s",g_string);
			if ((fp = fopen(g_string, "r")) != NULL){
				data.etat[num] +=18 ;
				++data.nb_rom;
				++flag_rom;
				printf("[");
				fclose(fp);
			}
			if ( flag_rom ){*/
				sprintf((char*)g_string, "./roms/%s.fba", data.zip[num] );
				//printf("%s",g_string);
				if ((fp = fopen(g_string, "r")) != NULL){
					data.etat[num] = BLEU ;
					++data.nb_cache;
					//printf("]");
					fclose(fp);
				//}
			} else data.etat[num] = JAUNE ;
		}else{ //sinon pas parent
			/*sprintf((char*)g_string, "./roms/%s.zip", data.zip[num] );
			printf("%s",g_string);
			if ((fp = fopen(g_string, "r")) != NULL){
				data.etat[num] +=9 ;
				++data.nb_rom;
				++flag_rom;
				printf("[");
				fclose(fp);
			}
			if ( flag_rom == 1){*/
				sprintf((char*)g_string, "%s/%s.zip", capex.rompath,data.parent[num] );
				//printf("%s",g_string);

				if ((fp = fopen(g_string, "r")) != NULL){
					data.etat[num] += 9;
					++flag_rom;
					printf("-");
					fclose(fp);
					test++;
				}
			//}
			if ( flag_rom == 1){
				sprintf((char*)g_string, "./roms/%s.fba", data.parent[num] );
				//printf("%s",g_string);
				if ((fp = fopen(g_string, "r")) != NULL){
					data.etat[num] += 9;
					//printf("]");
					fclose(fp);
				}
			}
		}
	}
	}
	printf("fin analyse set %d %d\n",test,test2);
}

char lecture_rominfo(void)
{
	unsigned int caractere;
	unsigned int temp;
	unsigned char flag_v0,flag_v1;

	char ligne[256];
	char arg1[256];
	char arg2[256];
	char arg3[256];

	int found=0;

	printf("Reading rominfo.fba\n");
	if ((fp = fopen("rominfo.fba", "r")) != NULL){
		//printf("[");
		while(fgets(ligne,sizeof(ligne),fp) != NULL){
			sscanf(ligne, "%s %s %s", &arg1 , &arg2 , &arg3 );
			if ( strcmp( arg1 , "FILENAME(" ) == 0 ){
				found=0;
				for ( ii=0 ; ii<data.nb_list[0] ; ++ii){

					if ( data.parent[ii] == NULL )
					if ( strncmp( data.zip[ii] , arg2 , (strlen(arg2)) ) == 0 && (strlen(arg2)) == strlen(data.zip[ii]) ){
						data.parent[ii] = (char*) calloc( strlen(arg3) , sizeof(char));
						strncpy( data.parent[ii] , arg3 , (strlen(arg3)) );
//						printf("(%s->%s)", data.zip[ii], data.parent[ii]);
						found=1;
						break;
					}
				}
				if(found==0) printf("(%s->unknown)",arg2);

			}//else printf("-");

		}fclose(fp);
		//printf("]\n");

	}

	//check parent rom absent
	for ( ii=0 ; ii<data.nb_list[0] ; ++ii)
		if ( data.parent[ii] == NULL ){
			data.parent[ii] = (char*) calloc( 7 +1 , sizeof(char));
			strncpy( data.parent[ii] , "unknown" , 7 );
		}

	return 0;
}

void load_preview(unsigned int numero)
{
	//preview
	if ( strcmp( data.parent[listing_tri[capex.list][numero]] , "fba" ) == 0 )
		sprintf((char*)g_string, "./preview/%s.bmp" , data.zip[listing_tri[capex.list][numero]]);
	else sprintf((char*)g_string, "./preview/%s.bmp" , data.parent[listing_tri[capex.list][numero]]);

	if ((fp = fopen(g_string, "r")) != NULL){
		Tmp = SDL_LoadBMP(g_string);
		preview = SDL_DisplayFormat(Tmp);
		SDL_FreeSurface(Tmp);
		fclose(fp);
		flag_preview = 1;
		drawSprite( bg , bg_temp , 124 , 3 , 124 , 3 , 192 , 112 );
		drawSprite(preview, bg_temp, 0, 0, 220-Tmp->w/2, 3, 192, 112 );
	}else{
		drawSprite( bg , bg_temp , 124 , 3 , 124 , 3 , 192 , 112 );

		sprintf((char*)g_string, "PREVIEW %s.bmp" , data.zip[listing_tri[capex.list][numero]]);
		put_string(g_string , 160 , 49 , ROUGE , bg_temp );
		put_string( "NOT AVAILABLE" , 181 , 59 , ROUGE , bg_temp );
		
		flag_preview = 0;
	}

}
