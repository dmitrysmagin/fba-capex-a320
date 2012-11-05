/*  CAPEX for GP2X

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

void write_cfg()
{
	printf("ecriture capex.cfg debut\n");
	fp = fopen("./capex.cfg", "w"); // FIX PATH HERE

	fputs("# CAPEX config file v1.1\n",fp);
	fputs("\n",fp);

	fputs("# FBA2X OPTIONS\n",fp);
	fputs("\n",fp);
	sprintf((char*)g_string, "fba_clock %d\n",options.cpu);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_sound %d\n",options.sound);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_samplerate %d\n",options.samplerate);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_rescale %d\n",options.rescale);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_showfps %d\n",options.showfps);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_frontend %d\n",options.frontend);
	fputs(g_string,fp);
	sprintf((char*)g_string, "m68k_core %d\n",options.m68kcore);
	fputs(g_string,fp);
	sprintf((char*)g_string, "z80_core %d\n",options.z80core);
	fputs(g_string,fp);
	sprintf((char*)g_string, "rotate %d\n",options.rotate);
	fputs(g_string,fp);

	sprintf((char*)g_string, "filter %d\n",options.filter);
	fputs(g_string,fp);

	sprintf((char*)g_string, "rotate %d\n",options.rotate);
	fputs(g_string,fp);

	sprintf((char*)g_string, "rescale %d\n",options.rescale);
	fputs(g_string,fp);

	sprintf((char*)g_string, "sensitivity %d\n",options.sense);
	fputs(g_string,fp);

	fputs("\n",fp);

	fputs("# CAPEX OPTIONS\n",fp);
	fputs("\n",fp);
	sprintf((char*)g_string, "capex_deadzone %d\n",capex.deadzone);
	fputs(g_string,fp);
	sprintf((char*)g_string, "capex_clock %d\n",capex.clock);
	fputs(g_string,fp);
	sprintf((char*)g_string, "capex_tweak %d\n",capex.tweak);
	fputs(g_string,fp);
	sprintf((char*)g_string, "capex_delay_speed %d\n",capex.delayspeed);
	fputs(g_string,fp);
	sprintf((char*)g_string, "capex_repeat_speed %d\n",capex.repeatspeed);
	fputs(g_string,fp);
	sprintf((char*)g_string, "capex_list %d\n",capex.list);
	fputs(g_string,fp);
	sprintf((char*)g_string, "capex_shadow %d\n",capex.FXshadow);
	fputs(g_string,fp);
	sprintf((char*)g_string, "capex_skin %d\n",capex.skin);
	fputs(g_string,fp);
	sprintf((char*)g_string, "rom_path %s\n",(char*)capex.rompath);
	fputs(g_string,fp);


	fclose(fp);
	sync();
	printf("ecriture capex.cfg fin\n");
}

void write_global_key()
{
	sprintf((char*)g_string, "./config/default.kmp", conf.cf);
	fp = fopen(g_string, "w");
	fprintf(fp,"KEY_LEFT %d\n",KEYLEFT);
	fprintf(fp,"KEY_RIGHT %d\n",KEYRIGHT);
	fprintf(fp,"KEY_UP %d\n",KEYUP);
	fprintf(fp,"KEY_DOWN %d\n",KEYDOWN);
	fprintf(fp,"KEY_FIRE1 %d\n",KEYA);
	fprintf(fp,"KEY_FIRE2 %d\n",KEYB);
	fprintf(fp,"KEY_FIRE3 %d\n",KEYX);
	fprintf(fp,"KEY_FIRE4 %d\n",KEYY);
	fprintf(fp,"KEY_START1 %d\n",KEYSTART);
	fprintf(fp,"KEY_COIN1 %d\n",KEYSELECT); 
	fprintf(fp,"KEY_FIRE5 %d\n",KEYLS);
	fprintf(fp,"KEY_FIRE6 %d\n",KEYRS);
	fprintf(fp,"KEY_QUIT %d\n",KEYQ);
	fprintf(fp,"KEY_PAUSE %d\n",KEYPAUSE);
	fclose(fp);
	sync();

}

void write_rom_key()
{
	sprintf((char*)g_string, "./config/%s.kmp", conf.cf);
	fp = fopen(g_string, "w");
	fprintf(fp,"KEY_LEFT %d\n",KEYLEFT);
	fprintf(fp,"KEY_RIGHT %d\n",KEYRIGHT);
	fprintf(fp,"KEY_UP %d\n",KEYUP);
	fprintf(fp,"KEY_DOWN %d\n",KEYDOWN);
	fprintf(fp,"KEY_FIRE1 %d\n",KEYA);
	fprintf(fp,"KEY_FIRE2 %d\n",KEYB);
	fprintf(fp,"KEY_FIRE3 %d\n",KEYX);
	fprintf(fp,"KEY_FIRE4 %d\n",KEYY);
	fprintf(fp,"KEY_START1 %d\n",KEYSTART);
	fprintf(fp,"KEY_COIN1 %d\n",KEYSELECT);
	fprintf(fp,"KEY_FIRE5 %d\n",KEYLS);
	fprintf(fp,"KEY_FIRE6 %d\n",KEYRS);
	fprintf(fp,"KEY_QUIT %d\n",KEYQ);
	fprintf(fp,"KEY_PAUSE %d\n",KEYPAUSE);
	fclose(fp);
	sync();

}

void write_cf()
{
	sprintf((char*)g_string, "./config/%s.cf", conf.cf);
	printf("ecriture %s debut\n", g_string);

	fp = fopen(g_string, "w");

	fputs("# Config V2 file generate by CAPEX\n",fp);
	sprintf((char*)g_string, "# %s\n", conf.cf );
	fputs(g_string,fp);
	fputs("\n",fp);
	sprintf((char*)g_string, "fba_clock %d\n",options.cpu);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_sound %d\n",options.sound);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_samplerate %d\n",options.samplerate);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_rescale %d\n",options.rescale);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_rotate %d\n",options.rotate);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_sensitivity %d\n",options.sense);
	fputs(g_string,fp);
	sprintf((char*)g_string, "fba_showfps %d\n",options.showfps);
	fputs(g_string,fp);
	sprintf((char*)g_string, "m68k_core %d\n",options.m68kcore);
	fputs(g_string,fp);
	sprintf((char*)g_string, "z80_core %d\n",options.z80core);
	fputs(g_string,fp);

    sprintf((char*)g_string, "rotate %d\n",options.rotate);
	fputs(g_string,fp);

	sprintf((char*)g_string, "filter %d\n",options.filter);
	fputs(g_string,fp);

	fclose(fp);
	sync();

	sprintf((char*)g_string, "./config/%s.cf", conf.cf);
	printf("ecriture %s fin\n", g_string);
}
