/*
 *  Copyright 2010 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  C5525 eZdsp LCD display
 *
 */

#include "app_globals.h"
#include "psp_i2s.h"
#include "psp_i2c.h"
#include "dda_i2c.h"
#include "psp_common.h"
#include "lcd_osd.h"
#include "stdio.h"
#include "string.h"
#include "dsplib.h"
#include "DMFX2_CSL_BIOS_cfg.h"
#include "dsp_fx.h"

#ifdef C5535_EZDSP_DEMO

#define OSD9616_I2C_ADDR 0x3C    // OSD9616 I2C address

#pragma DATA_SECTION(CharGen_6x8, ".charrom")
Uint16 CharGen_6x8[256*3];

extern PSP_Handle    hi2c;

struct MenuTable FxMenu =
{
	EFFECT_OFF, 
	5, 
	{DIST_PARAM_OFF, SHOEGAZE, 8, 3, 8}, 
	{EQ_TYPE_OFF, 0, 0, 0, 0}, 
	{PHASER_PARAM_OFF, 0, 0, 0}, 
	REVERB_TYPE_OFF
};

char InitMenu [4][16] = {
		    "    Rezzonics   ",
		    "      DMFX-1    ",
		    "       v1.0     ",
		    "       2015     "};

typedef struct ContextMenu_t {
    char MenuStr[16];
    ContextMenu_t *LeftContext;
    ContextMenu_t *RightContext;
    ContextMenu_t *DownContext;
    ContextMenu_t *UpContext;
    ContextMenu_t *CenterContext;
} ContextMenu_t;

ContextMenu_t MenuTop [14] =
{
	{"1 Main settings ", NULL, MenuMainSet[0],  MenuTop[1],  MenuTop[14], MenuTop[0]},
    {"2 Patch select  ", NULL, MenuPatchSet[0], MenuTop[2],  MenuTop[0],  MenuTop[1]},
    {"3  Compressor   ", NULL, MenuComp[0],     MenuTop[3],  MenuTop[1],  MenuTop[2]},
    {"4  Distortion   ", NULL, MenuDist[0],     MenuTop[4],  MenuTop[2],  MenuTop[3]},
    {"5  Equalizer    ", NULL, MenuEqParam[0],  MenuTop[5],  MenuTop[3],  MenuTop[4]},
    {"6  Phaser       ", NULL, MenuPhaser[0],   MenuTop[6],  MenuTop[4],  MenuTop[5]},
    {"7  Noise Cancel ", NULL, MenuNoise[0],    MenuTop[7],  MenuTop[5],  MenuTop[6]},
    {"8  Wah-Wah      ", NULL, NULL,            MenuTop[8],  MenuTop[6],  MenuTop[7]},
    {"9  Delay        ", NULL, MenuDelay[0],    MenuTop[9],  MenuTop[7],  MenuTop[8]},
    {"10 Chorus       ", NULL, MenuChorus[0],   MenuTop[10], MenuTop[8],  MenuTop[9]},
    {"11 Flanger      ", NULL, MenuFlanger[0],  MenuTop[11], MenuTop[9],  MenuTop[10]},
    {"12 Phase Shift  ", NULL, MenuPhShift[0],  MenuTop[12], MenuTop[10], MenuTop[11]},
    {"13 Tremolo      ", NULL, MenuTremolo[0],  MenuTop[13], MenuTop[11], MenuTop[12]},
    {"14 Reverb       ", NULL, MenuReverb[0],   MenuTop[14], MenuTop[12], MenuEffect[13]}
};

ContextMenu_t MenuMainSet [3] = 
{
    {"1  Bypass       ", MenuTop[0], NULL, MenuMainSet[1], MenuMainSet[2], MenuMainSet[0]},
    {"2  Main Volume  ", MenuTop[0], NULL, MenuMainSet[2], MenuMainSet[0], MenuMainSet[1]},
    {"3  Sampling Freq", MenuTop[0], NULL, MenuMainSet[0], MenuMainSet[1], MenuMainSet[2]}
};
ContextMenu_t MenuPatchSet [3] = {
	{"1  Patch       ", MenuTop[1], NULL, MenuPatchSet[1], MenuPatchSet[2], MenuPatchSet[0]},
	{"2  Patch       ", MenuTop[1], NULL, MenuPatchSet[2], MenuPatchSet[0], MenuPatchSet[1]},
	{"3  Patch       ", MenuTop[1], NULL, MenuPatchSet[0], MenuPatchSet[1], MenuPatchSet[2]}
};

ContextMenu_t MenuComp [3] = {
	{"1  Sustain      ", MenuTop[2], NULL, MenuComp[1], MenuComp[2], MenuComp[0]},
	{"2  Attack       ", MenuTop[2], NULL, MenuComp[2], MenuComp[0], MenuComp[1]},
	{"3  Level        ", MenuTop[2], NULL, MenuComp[0], MenuComp[1], MenuComp[2]}
};

ContextMenu_t MenuDist [4] = {
	{"1  Type         ", MenuTop[3], MenuDistType[0], MenuDist[1], MenuDist[3], MenuDist[0]},
	{"2  Drive        ", MenuTop[3], NULL, 		   MenuDist[2], MenuDist[0], MenuDist[1]},
	{"3  Tone         ", MenuTop[3], NULL, 		   MenuDist[3], MenuDist[1], MenuDist[2]},
	{"4  Level        ", MenuTop[3], NULL, 		   MenuDist[0], MenuDist[2], MenuDist[3]}
};

ContextMenu_t MenuDistType [7] = {
	{"1  Blues        ", MenuDist[0], NULL, MenuDistType[1], MenuDistType[6], MenuDistType[0]},
	{"2  Warm         ", MenuDist[0], NULL, MenuDistType[2], MenuDistType[0], MenuDistType[1]},
	{"3  Crunch       ", MenuDist[0], NULL, MenuDistType[3], MenuDistType[1], MenuDistType[2]},
	{"4  Vintage      ", MenuDist[0], NULL, MenuDistType[4], MenuDistType[2], MenuDistType[3]},
	{"5  Hard         ", MenuDist[0], NULL, MenuDistType[5], MenuDistType[3], MenuDistType[4]},
	{"6  Metal        ", MenuDist[0], NULL, MenuDistType[6], MenuDistType[4], MenuDistType[5]},
	{"7  Fuzz         ", MenuDist[0], NULL, MenuDistType[0], MenuDistType[5], MenuDistType[6]}
   	//"8  Shoegaze     ", MenuDist[0], NULL, MenuDistType[1], MenuDistType[1],
};

ContextMenu_t MenuEqParam [4] = {
	{"1  Treble       ",  MenuTop[4], NULL, MenuEqParam[1], MenuEqParam[3], MenuEqParam[0]},
	{"2  Middle       ",  MenuTop[4], NULL, MenuEqParam[2], MenuEqParam[0], MenuEqParam[1]},
	{"3  Bass         ",  MenuTop[4], NULL, MenuEqParam[3], MenuEqParam[1], MenuEqParam[2]},
	{"4  Level        ",  MenuTop[4], NULL, MenuEqParam[0], MenuEqParam[2], MenuEqParam[3]}
};

ContextMenu_t MenuPhaser [3] = {
	{"1  Rate         ",  MenuTop[5], NULL, MenuPhaser[1], MenuPhaser[2], MenuPhaser[0]},
	{"2  Depth        ",  MenuTop[5], NULL, MenuPhaser[2], MenuPhaser[0], MenuPhaser[1]},
	{"3  Resonance    ",  MenuTop[5], NULL, MenuPhaser[0], MenuPhaser[1], MenuPhaser[2]}
};

ContextMenu_t MenuNoise [2] = {
	{"1  Threshold    ", MenuTop[6], NULL, MenuNoise[1], MenuNoise[1], MenuNoise[0]},
	{"2  Auto         ", MenuTop[6], NULL, MenuNoise[0], MenuNoise[0], MenuNoise[1]}
};

ContextMenu_t MenuDelay [4] = {
		{"1  Type         ", MenuTop[8], NULL, MenuDelay[1], MenuDelay[3], MenuDelay[0]},
		{"2  Time         ", MenuTop[8], NULL, MenuDelay[2], MenuDelay[0], MenuDelay[1]},
		{"3  Feedback     ", MenuTop[8], NULL, MenuDelay[3], MenuDelay[1], MenuDelay[2]},
		{"4  Level        ", MenuTop[8], NULL, MenuDelay[0], MenuDelay[2], MenuDelay[3]}
};
ContextMenu_t MenuChorus [3] = {
		{"1  Rate         ", MenuTop[9], NULL, MenuChorus[1], MenuChorus[2], MenuChorus[0]},
		{"2  Depth        ", MenuTop[9], NULL, MenuChorus[2], MenuChorus[0], MenuChorus[1]},
		{"3  Level        ", MenuTop[9], NULL, MenuChorus[0], MenuChorus[1], MenuChorus[2]}
};
ContextMenu_t MenuFlanger [3] = {
		{"1  Rate         ", MenuTop[10], NULL, MenuFlanger[1], MenuFlanger[2], MenuFlanger[0]},
		{"2  Depth        ", MenuTop[10], NULL, MenuFlanger[2], MenuFlanger[0], MenuFlanger[1]},
		{"3  Resonance    ", MenuTop[10], NULL, MenuFlanger[0], MenuFlanger[1], MenuFlanger[2]}
};
ContextMenu_t MenuPhShift [3] = {
		{"1  Pitch        ", MenuTop[11], NULL, MenuPhShift[1], MenuPhShift[2], MenuPhShift[0]},
		{"2  Direct Level ", MenuTop[11], NULL, MenuPhShift[2], MenuPhShift[0], MenuPhShift[1]},
		{"3  Effect level ", MenuTop[11], NULL, MenuPhShift[0], MenuPhShift[1], MenuPhShift[2]}
};
ContextMenu_t MenuTremolo [2] = {
		{"1  Rate         ", MenuTop[12], NULL, MenuTremolo[1], MenuTremolo[1], MenuTremolo[0]},
		{"2  Depth        ", MenuTop[12], NULL, MenuTremolo[0], MenuTremolo[0], MenuTremolo[1]}
};
ContextMenu_t MenuReverb [3] = {
		{"1  Type         ", MenuTop[13], NULL, MenuReverb[1], MenuReverb[2], MenuReverb[0]},
		{"2  Time         ", MenuTop[13], NULL, MenuReverb[2], MenuReverb[0], MenuReverb[1]},
		{"3  Effect level ", MenuTop[13], NULL, MenuReverb[0], MenuReverb[1], MenuReverb[2]}
};


char MenuValString [23] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

short *A_dist[4], N_dist[4];

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 OSD9616_send( Uint16 regAddr, Uint16 regData, PSP_Handle hi2c )                        *
 *                                                                          *
 *      Sends 2 bytes of data to the OSD9616                                *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 OSD9616_send( Uint16 regAddr, Uint16 regData)
{
    PSP_Result    status;
    Uint16        writeCount;
    Uint16        writeBuff[2];

    if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        status = I2C_Write(hi2c, OSD9616_I2C_ADDR, writeCount, writeBuff);
    }

    return status;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 OSD9616_multiSend( Uint16 regAddr, Uint16 *regData, Uint16 length, PSP_Handle hi2c )                   *
 *                                                                          *
 *      Sends multiple bytes of data to the OSD9616                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 OSD9616_multiSend( Uint16 *regData, Uint16 length, PSP_Handle hi2c )
{
    PSP_Result    status;
    Uint16        i;
    Uint16        writeBuff[10];

    if(hi2c != NULL)
    {

        for (i=0; i<length; i++) {
        	writeBuff[i] = (regData[i] & 0x00FF);
        }

        /* Write the data */
        status = I2C_Write(hi2c, OSD9616_I2C_ADDR, length, writeBuff);
    }
    return status;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 printstring(char *a)              *
 *                                                                          *
 *      print a string                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 clear_lcd()
{
	Uint16 i;

    /* clear page 0 */ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5		
	for (i=0; i<128; i++) {
		OSD9616_send(0x40,0x00);
	}
    /* clear page 1 */ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5	
    for (i=0; i<128; i++) {
		OSD9616_send(0x40,0x00);
	}
	return 0;	
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 print_ti()                                                       *
 *                                                                          *
 *      Print "Texas Instruments"                            				*
 *            "C5535 eZdsp stk"                                                              *
 * ------------------------------------------------------------------------ */
Int16 print_ti()
{
	char ti[23]; 
		
	OSD9616_send(0x00,0x2e); // Turn off scrolling 
	clear_lcd();
	sprintf(ti,"                       "); //clear	
	    /* Write to page 1 */
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
   	
   	sprintf(ti,"  Texas Instruments");
	printstring(ti);
	    /* Write to page 0*/ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
	sprintf(ti," C5535 eZdsp USB stick");
	printstring(ti);
	
	OSD9616_send(0x00,0x2f); // Turn on scrolling 
	
	return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 print_dsp()                                                       *
 *                                                                          *
 *      Print "Industry lowest"                            				*
 *            "power dsp C5535"                                                             *
 * ------------------------------------------------------------------------ */
Int16 print_dsp()
{
	char ti[23]; 
	Uint16 i;
	
	OSD9616_send(0x00,0x2e); // Turn off scrolling 
	clear_lcd();
	
	for (i=0; i<23; i++) 
	  ti[i] = 0x20;      //clear buffer
	  
	//sprintf(ti,"                       "); //clear	
	    /* Write to page 1 */
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
   	
   	sprintf(ti,"  Industry's lowest");
	printstring(ti);
	    /* Write to page 0*/ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
	sprintf(ti,"  power DSP C5535");
	printstring(ti);
	
	OSD9616_send(0x00,0x2f); // Turn on scrolling 
	return 0;
}

short SetDistParams(void)
{
#if 0
	switch(FxMenu.Distortion.Type)
	{
	case BLUES:
 		A_dist[0] = A_tube2[0]; A_dist[1] = A_tube2[1]; A_dist[2] = A_tube2[2]; A_dist[3] = A_tube2[3];
		N_dist[0] = 2; N_dist[1] = 3; N_dist[2] = 3; N_dist[3] = 1;
		break; 
	case WARM:
 		A_dist[0] = A_gdist2[0]; A_dist[1] = A_gdist2[1]; A_dist[2] = A_gdist2[2]; A_dist[3] = A_gdist2[3];
		N_dist[0] = 2; N_dist[1] = 4; N_dist[2] = 3; N_dist[3] = 1;
		break; 
	case CRUNCH:
 		A_dist[0] = A_exp; A_dist[1] = A_exp; A_dist[2] = A_exp; A_dist[3] = A_exp;
		N_dist[0] = 2; N_dist[1] = 3; N_dist[2] = 3; N_dist[3] = 1;
		break;
	case VINT:
 		A_dist[0] = A_gdist[0]; A_dist[1] = A_gdist[2]; A_dist[2] = A_gdist[4]; A_dist[3] = A_gdist[6];
		N_dist[0] = 2; N_dist[1] = 4; N_dist[2] = 3; N_dist[3] = 1;
		break;
	case HARD:
 		A_dist[0] = A_atan[4]; A_dist[1] = A_atan[4]; A_dist[2] = A_atan[5]; A_dist[3] = A_atan[5];
		N_dist[0] = 3; N_dist[1] = 5; N_dist[2] = 5; N_dist[3] = 2;
		break;
	case METAL:
 		A_dist[0] = A_atan[6]; A_dist[1] = A_atan[7]; A_dist[2] = A_atan[8]; A_dist[3] = A_atan[9];
		N_dist[0] = 4; N_dist[1] = 5; N_dist[2] = 5; N_dist[3] = 2;
		break;
	case FUZZ:
 		A_dist[0] = A_fuzz2; A_dist[1] = A_fuzz2; A_dist[2] = A_fuzz2; A_dist[3] = A_fuzz2;
		N_dist[0] = 5; N_dist[1] = 5; N_dist[2] = 5; N_dist[3] = 2;
		break;
	case SHOEGAZE:
	default:
 		A_dist[0] = A_assy2; A_dist[1] = A_assy2; A_dist[2] = A_assy2; A_dist[3] = A_assy2;
		N_dist[0] = 5; N_dist[1] = 3; N_dist[2] = 3; N_dist[3] = 2;
		break;
	}	
	return(0);
#endif
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 menu()                                                            *
 *                                                                          *
 *      Print "   Menu    "                              				    *
 * ------------------------------------------------------------------------ */

short menu(LcdMsgType LcdMsg)
{
	char s1[23], s2[23]; //= {0, 1, 2, 3, 4, 5, 6, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}; 
	Int16 i;
	static MenuStatType MenuStat = INIT;
	
	switch(MenuStat)
	{
	case INIT:  // Initial status any button passes to Main Level
		if ((LcdMsg == LCD_MSG_BUTTON_L) || (LcdMsg == LCD_MSG_BUTTON_R))
		{
			MenuStat = VOL0;
			FxMenu.Effect = VOLUME;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Display Main Level
			//LOG_printf(&trace, "INIT - LR - Main Level\n");
		}
		break;
	case VOL0:  // Main Level menu. L Button set volume, R goes to Distortion menu
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = VOL1;
			FxMenu.Volume = 0; 
			print_two(MenuEffect[FxMenu.Effect-1], "................");  // Set volume
			//LOG_printf(&trace, "VOL0 - L - Main Level\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = DIST0;
			FxMenu.Effect = DISTORTION;
			FxMenu.Distortion.Param = DIST_PARAM_OFF;
			print_two(MenuEffect[FxMenu.Effect-1], "                "); // Skips to Distortion menu
			//LOG_printf(&trace, "VOL0 - R - Distortion\n");
		}
		break;
	case VOL1:  // Sets Main Level
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DIST0;
			FxMenu.Distortion.Param = DIST_PARAM_OFF;
			FxMenu.Effect = DISTORTION;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Once set volume goes to dist menu
			//LOG_printf(&trace, "VOL1 - L - Distortion\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Volume < 10) FxMenu.Volume += 1;
			else FxMenu.Volume = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Volume)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuEffect[FxMenu.Effect-1], s1);  // Adjust main level. Show vol cursor
			//LOG_printf(&trace, "VOL1 - R - Set Volume\n");
		}
		break;
	case DIST0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTTYPE0;
			FxMenu.Distortion.Param = TYPE;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");  // Enter into dist menu first param: Type
			//LOG_printf(&trace, "DIST0 - L - Type\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			FxMenu.Effect = EQUALIZER;
			MenuStat = EQ0;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Skips to Equalizer menu
			//LOG_printf(&trace, "DIST0 - R - Equalizer\n");
		}
		break;
	case DISTTYPE0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTTYPE1;
			FxMenu.Distortion.Type = TYPE;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "......................");  // Set distortion type
			//LOG_printf(&trace, "DISTTYPE0 - L - Set Type\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = DISTDRIVE0;
			FxMenu.Distortion.Param = DRIVE;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");  // Skips to Distortion Drive
			//LOG_printf(&trace, "DISTTYPE0 - R - Drive\n");
		}
		break;
	case DISTTYPE1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTDRIVE0;
			FxMenu.Distortion.Param = DRIVE;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");  // once dist type is set skips to dist Drive
			//LOG_printf(&trace, "DISTTYPE1 - L - Drive\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Distortion.Type < 8) FxMenu.Distortion.Type += 1;
			else FxMenu.Distortion.Type = 1;
			print_two(MenuDist[FxMenu.Distortion.Param-1], MenuDistType[FxMenu.Distortion.Type-1]); // Adjust dist type
			//LOG_printf(&trace, "DISTTYPE1 - R - Type\n");
		}
		break;
	case DISTDRIVE0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTDRIVE1;
			FxMenu.Distortion.Drive = 0;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "......................");  // Set Dist Drive
			//LOG_printf(&trace, "DISTDRIVE0 - L - Set Drive\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = DISTTONE0;
			FxMenu.Distortion.Param = TONE;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");  // Skip to dist tone 
			//LOG_printf(&trace, "DISTDRIVE0 - R - Tone\n");
		}
		break;
	case DISTDRIVE1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTTONE0;
			FxMenu.Distortion.Param = TONE;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      "); // Once drive is set skips to dist tone
			//LOG_printf(&trace, "DISTDRIVE1 - L - Tone\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Distortion.Drive<10) FxMenu.Distortion.Drive += 1;
			else FxMenu.Distortion.Drive = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Distortion.Drive)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);  // Adjust dist drive
			//LOG_printf(&trace, "DISTDRIVE1 - R - Drive\n");
		}
		break;
	case DISTTONE0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTTONE1;
			FxMenu.Distortion.Tone = 0;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "......................");  // Set dist tone
			//LOG_printf(&trace, "DISTTONE0 - L - Set Tone\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = DISTLEVEL0;
			FxMenu.Distortion.Param = LEVEL;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");  // Skip to Dist Level
			//LOG_printf(&trace, "DISTTONE0 - R - Level\n");
		}
		break;
	case DISTTONE1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTLEVEL0;
			FxMenu.Distortion.Param = LEVEL;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      "); // Once tone is set skips to Level
			//LOG_printf(&trace, "DISTTONE1 - L - Level\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Distortion.Tone<10) FxMenu.Distortion.Tone += 1;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Distortion.Tone)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);  // adjust tone
			//LOG_printf(&trace, "DISTTONE1 - R - Tone\n");
		}
		break;
	case DISTLEVEL0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = DISTLEVEL1;
			FxMenu.Distortion.Level = 0;
			print_two(MenuDist[FxMenu.Distortion.Param-1], "......................");  //Set level
			//LOG_printf(&trace, "DISTLEVEL0 - L - Set Level\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = EQ0;
			FxMenu.Effect = EQUALIZER;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Skip to Equalizer
			//LOG_printf(&trace, "DISTLEVEL0 - R - Equalizer\n");
		}
		break;
	case DISTLEVEL1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = EQ0;
			FxMenu.Effect = EQUALIZER;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Once level is set skip to Equalizer
			//LOG_printf(&trace, "DISTLEVEL1 - L - Equalizer\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Distortion.Level < 10) FxMenu.Distortion.Level += 1;
			else FxMenu.Distortion.Level = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Distortion.Level)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);  // adjust level
			//LOG_printf(&trace, "DISTLEVEL1 - R - Level\n");
		}
		break;
	case EQ0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = EQTREBLE0;
			FxMenu.Equalizer.Param = TREBLE;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                ");  //Set treble
			//LOG_printf(&trace, "EQ0 - L - Treble\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = PHASER0;
			FxMenu.Effect = PHASER;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Skips to phaser
			//LOG_printf(&trace, "EQ0 - R - Phaser\n");
		}
		break;
	case EQTREBLE0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = EQTREBLE1;
			FxMenu.Equalizer.Treble = 0;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "................");  // Set treble
			//LOG_printf(&trace, "EQTREBLE0 - L - Set Treble\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = EQMIDDLE0;
			FxMenu.Equalizer.Param = MIDDLE;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                ");  // skips to middle
			//LOG_printf(&trace, "EQTREBLE0 - R - Middle\n");
		}
		break;
	case EQTREBLE1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = EQMIDDLE0;
			FxMenu.Equalizer.Param = MIDDLE;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                "); //once treble is set skip to middle
			//LOG_printf(&trace, "EQTREBLE1 - L - Middle\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Equalizer.Treble < 10) FxMenu.Equalizer.Treble += 1;
			else FxMenu.Equalizer.Treble = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Equalizer.Treble)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);  // adjust treble
			//LOG_printf(&trace, "EQTREBLE1 - R - Treble\n");
		}
		break;
	case EQMIDDLE0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = EQMIDDLE1;
			FxMenu.Equalizer.Middle = 0;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "................");  // Set middle
			//LOG_printf(&trace, "EQMIDDLE0 - L - Set Middle\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = EQBASS0;
			FxMenu.Equalizer.Param = BASS;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                ");  // Skips to Bass
			//LOG_printf(&trace, "EQMIDDLE0 - R - Bass\n");
		}
		break;
	case EQMIDDLE1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = EQBASS0;
			FxMenu.Equalizer.Param = BASS;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                ");
			//LOG_printf(&trace, "EQMIDDLE1 - L - Bass\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Equalizer.Middle<10) FxMenu.Equalizer.Middle += 1;
			else FxMenu.Equalizer.Middle = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Equalizer.Middle)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
			//LOG_printf(&trace, "EQMIDDLE1 - R - Middle\n");
		}
		break;
	case EQBASS0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = EQBASS1;
			FxMenu.Equalizer.Bass = 0;
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "................");
			//LOG_printf(&trace, "EQBASS0 - L - Set Bass\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = PHASER0;
			FxMenu.Effect = PHASER;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Skip to Phaser
			//LOG_printf(&trace, "EQBASS0 - R - Phaser\n");
		}
		break;
	case EQBASS1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = PHASER0;
			FxMenu.Effect = PHASER;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");  // Skip to Phaser
			//LOG_printf(&trace, "EQBASS1 - L - Phaser\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Equalizer.Bass<10) FxMenu.Equalizer.Bass += 1;
			else FxMenu.Equalizer.Bass = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Equalizer.Bass)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
			//LOG_printf(&trace, "EQBASS1 - R - Bass\n");
		}
		break;
	case PHASER0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = PHRATE0;
			FxMenu.Phaser.Param = RATE;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                ");
			//LOG_printf(&trace, "PHASER0 - L - Rate\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = VOL0;
			FxMenu.Effect = VOLUME;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");
            //SEM_post(&SEM_DmaRxRightComplete);

			//LOG_printf(&trace, "PHASER0 - R - Main level\n");
		}
		break;
	case PHRATE0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = PHRATE1;
			FxMenu.Phaser.Rate = 0;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "................");
			//LOG_printf(&trace, "PHRATE0 - L - Phaser Rate\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = PHDEPTH0;
			FxMenu.Phaser.Param = DEPTH;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                ");
			//LOG_printf(&trace, "PHRATE0 - R - Phaser Depth\n");
		}
		break;
	case PHRATE1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = PHDEPTH0;
			FxMenu.Phaser.Param = DEPTH;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                ");
			//LOG_printf(&trace, "PHRATE1 - L - Phaser Depth\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Phaser.Rate<10) FxMenu.Phaser.Rate += 1;
			else FxMenu.Phaser.Rate = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Phaser.Rate)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], s1);
			//LOG_printf(&trace, "PHRATE1 - R - Phaser Rate\n");
		}
		break;
	case PHDEPTH0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = PHDEPTH1;
			FxMenu.Phaser.Depth = 0;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "................");
			//LOG_printf(&trace, "PHDEPTH0 - L - Phaser Depth\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = PHRES0;
			FxMenu.Phaser.Param = RESONANCE;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                ");
			//LOG_printf(&trace, "PHDEPTH0 - R - Phaser Resonance\n");
		}
		break;
	case PHDEPTH1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = PHRES0;
			FxMenu.Phaser.Param = RESONANCE;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                ");
			//LOG_printf(&trace, "PHDEPTH1 - L - Phaser Resonance\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Phaser.Depth<10) FxMenu.Phaser.Depth += 1;
			else FxMenu.Phaser.Depth = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Phaser.Depth)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], s1);
			//LOG_printf(&trace, "PHDEPTH1 - R - Phaser Depth\n");
		}
		break;
	case PHRES0:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = PHRES1;
			FxMenu.Phaser.Rate = 0;
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "................");
			//LOG_printf(&trace, "PHRES0 - L - Phaser Resonance\n");
		 }
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			MenuStat = INIT;
			FxMenu.Effect = EFFECT_OFF;
			print_two(MenuEffect[FxMenu.Effect-1], "                ");
            //SEM_post(&SEM_DmaRxRightComplete);
			//LOG_printf(&trace, "PHRES0 - R - Init\n");
		}
		break;
	case PHRES1:
		if (LcdMsg == LCD_MSG_BUTTON_L)
		{
			MenuStat = INIT;
			FxMenu.Phaser.Param = EFFECT_OFF;
			print_two(InitMenu[0], InitMenu[1]);
            //SEM_post(&SEM_DmaRxRightComplete);
			//LOG_printf(&trace, "PHRES1 - L - Init\n");
		}
		else if (LcdMsg == LCD_MSG_BUTTON_R)
		{
			if(FxMenu.Phaser.Resonance<10) FxMenu.Phaser.Resonance += 1;
			else FxMenu.Phaser.Resonance = 0;
			for(i=0; i < 23; i++) {
				if(i < FxMenu.Phaser.Resonance)
					s1[i] = MenuValString[i];
				else 
					s1[i] = ' ';
			} 
			print_two(MenuPhaser[FxMenu.Phaser.Param-1], s1);
			//LOG_printf(&trace, "PHRES1 - R - Phaser Resonance\n");
		}
		break;
	default:
			//LOG_printf(&trace, "What the fuck is this??\n");
		break;
	}
	return 0;
}

#if 0 
Int16 menu()
{
    LcdMsgObj      LcdMsg;
	char s1[23]; //= {0, 1, 2, 3, 4, 5, 6, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}; 
	Int16 i;
      
   	print_two(InitMenu[0], InitMenu[1]);
    while (1)
    {
    	// Wait for a button to be pressed
        if (MBX_pend(&MBX_lcdmsg, &LcdMsg, SYS_FOREVER))
        {
        	if (FxMenu.Effect == EFFECT_OFF) 
        	{
        		FxMenu.Effect = VOLUME;
        		print_two(MenuEffect[FxMenu.Effect-1], "                ");
        	}
            else 
            {  
	        	if (FxMenu.Effect == VOLUME) 
	        	{
               		if (FxMenu.Volume == 0)
                	{
	                	if (LcdMsg == LCD_MSG_BUTTON_L)
                		{
		        			FxMenu.Volume +=1;
		        			for(i=0; i < 23; i++) {
		        				if(i < FxMenu.Volume)
		        					s1[i] = MenuValString[i];
		        				else 
		        					s1[i] = ' ';
		        			} 
		        			//strncpy(s1, MenuValString, FxMenu.Volume+1);
		        			//strncat(s1, "                      ", 23 - FxMenu.Volume-1);
		        			print_two(MenuEffect[FxMenu.Effect-1], s1);
                		}
                		else if (LcdMsg == LCD_MSG_BUTTON_R)
                		{
	        				FxMenu.Effect = DISTORTION;
		        			print_two(MenuEffect[FxMenu.Effect-1], "                      ");
                		}
                	}
            		else if (LcdMsg == LCD_MSG_BUTTON_R)
            		{ 
	        			FxMenu.Volume +=1;
	        			for(i=0; i < 23; i++) {
	        				if(i < FxMenu.Volume)
	        					s1[i] = MenuValString[i];
	        				else 
	        					s1[i] = ' ';
	        			} 
	        			//strncpy(s1, MenuValString, FxMenu.Volume+1);
	        			//strncat(s1, "                      ", 23 - FxMenu.Volume-1);
	        			print_two(MenuEffect[FxMenu.Effect-1], s1);
            		}
            		else if (LcdMsg == LCD_MSG_BUTTON_L)
            		{
        				FxMenu.Effect = DISTORTION;
	        			print_two(MenuEffect[FxMenu.Effect-1], "                      ");
            		}
	        	}
	        	else if (FxMenu.Effect == DISTORTION) 
	        	{ 
	        		if (FxMenu.Distortion.Param == DIST_PARAM_OFF)
	        		{ 
	                	if (LcdMsg == LCD_MSG_BUTTON_L)
	                	{
		        			FxMenu.Distortion.Param += 1;
		        			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");
	                	}
	            		else if (LcdMsg == LCD_MSG_BUTTON_R)
	            		{ 
	        				FxMenu.Effect = EQUALIZER;
		        			print_two(MenuEffect[FxMenu.Effect-1], "                      ");
	            		}
	        		}
	        		else if (FxMenu.Distortion.Param == TYPE)
	        		{ 
	        			if (FxMenu.Distortion.Type == DIST_TYPE_OFF)
	        			{
	                		if (LcdMsg == LCD_MSG_BUTTON_L)
	                		{
		        				FxMenu.Distortion.Type += 1;
			        			print_two(MenuDistType[FxMenu.Distortion.Type-1], "                      ");
	                		}
	            			else if (LcdMsg == LCD_MSG_BUTTON_R)
	            			{ 
		        				FxMenu.Distortion.Param = DRIVE;
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");
	            			}
	        			}
	        			else
	        			{
	                		if (LcdMsg == LCD_MSG_BUTTON_R)
	                		{
		        				FxMenu.Distortion.Type += 1;
			        			print_two(MenuDistType[FxMenu.Distortion.Type-1], "                      ");
	                		}
	            			else if (LcdMsg == LCD_MSG_BUTTON_L)
	            			{ 
		        				FxMenu.Distortion.Param = DRIVE;
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");
	            			}
	        			}
	        		}
					else if (FxMenu.Distortion.Param == DRIVE)
					{
	        			if (FxMenu.Distortion.Drive == 0) 
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L) {
			        			FxMenu.Distortion.Drive += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Distortion.Drive)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Distortion.Drive);
			        			//strncat(s1, "                      ", 23 - FxMenu.Distortion.Drive);
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{ 
			        			FxMenu.Distortion.Param = TONE;
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");
		            		}
		        		}
		        		else
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_R) {
			        			FxMenu.Distortion.Drive += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Distortion.Drive)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Distortion.Drive);
			        			//strncat(s1, "                      ", 23 - FxMenu.Distortion.Drive);
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{ 
			        			FxMenu.Distortion.Param = TONE;
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");
		            		}
		        		}
	        		}
					else if (FxMenu.Distortion.Param == TONE)
					{
	        			if (FxMenu.Distortion.Tone == 0) 
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Distortion.Tone += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Distortion.Tone)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Distortion.Tone);
			        			//strncat(s1, "                      ", 23 - FxMenu.Distortion.Tone);
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{ 
			        			FxMenu.Distortion.Param = LEVEL;
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Distortion.Tone += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Distortion.Tone)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Distortion.Tone);
			        			//strncat(s1, "                      ", 23 - FxMenu.Distortion.Tone);
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{ 
			        			FxMenu.Distortion.Param = LEVEL;
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], "                      ");
		            		}
		        		}
	        		}
					else if (FxMenu.Distortion.Param == LEVEL)
					{
	        			if (FxMenu.Distortion.Level == 0) 
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Distortion.Level += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Distortion.Level)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Distortion.Level);
			        			//strncat(s1, "                      ", 23 - FxMenu.Distortion.Level);
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);
		        			}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{
			    	   			FxMenu.Distortion.Param = DIST_PARAM_OFF;
	        					print_two(MenuEffect[FxMenu.Effect-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Distortion.Level += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Distortion.Level)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Distortion.Level);
			        			//strncat(s1, "                      ", 23 - FxMenu.Distortion.Level);
			        			print_two(MenuDist[FxMenu.Distortion.Param-1], s1);
		        			}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{
			    	   			FxMenu.Distortion.Param = DIST_PARAM_OFF;
	        					print_two(MenuEffect[FxMenu.Effect-1], "                      ");
		            		}
		        		}
	        		}
	        	}
	        	else if (FxMenu.Effect == EQUALIZER) 
	        	{
	        		if (FxMenu.Equalizer.Param == EQ_TYPE_OFF)
	        		{ 
	                	if (LcdMsg == LCD_MSG_BUTTON_L)
	                	{
		        			FxMenu.Equalizer.Param += 1;
		        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                      ");
	                	}
	            		else if (LcdMsg == LCD_MSG_BUTTON_R)
	            		{
	        				FxMenu.Effect = PHASER;
		        			print_two(MenuEffect[FxMenu.Effect-1], "                      ");
	            		}
	        		}
	        		else if (FxMenu.Equalizer.Param == TREBLE)
	        		{ 
	        			if (FxMenu.Equalizer.Treble == 0) 
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Equalizer.Treble += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Equalizer.Treble)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Equalizer.Treble);
			        			//strncat(s1, "                      ", 23 - FxMenu.Equalizer.Treble);
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{ 
		        				FxMenu.Equalizer.Param = MIDDLE;
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Equalizer.Treble += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Equalizer.Treble)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Equalizer.Treble);
			        			//strncat(s1, "                      ", 23 - FxMenu.Equalizer.Treble);
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{ 
		        				FxMenu.Equalizer.Param = MIDDLE;
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                      ");
		            		}
		        		}
	        		}
	        		if (FxMenu.Equalizer.Param == MIDDLE)
	        		{ 
	        			if (FxMenu.Equalizer.Middle == 0) 
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Equalizer.Middle += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Equalizer.Middle)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Equalizer.Middle);
			        			//strncat(s1, "                      ", 23 - FxMenu.Equalizer.Middle);
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{ 
		        				FxMenu.Equalizer.Param = BASS;
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Equalizer.Middle += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Equalizer.Middle)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Equalizer.Middle);
			        			//strncat(s1, "                      ", 23 - FxMenu.Equalizer.Middle);
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{ 
		        				FxMenu.Equalizer.Param = BASS;
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], "                      ");
		            		}
		        		}
	        		}
	        		if (FxMenu.Equalizer.Param == BASS)
	        		{ 
	        			if (FxMenu.Equalizer.Middle == 0) 
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Equalizer.Bass += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Equalizer.Bass)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Equalizer.Bass);
			        			//strncat(s1, "                      ", 23 - FxMenu.Equalizer.Bass);
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{ 
		        				FxMenu.Equalizer.Param = EQ_TYPE_OFF;
	        					print_two(MenuEffect[FxMenu.Effect-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Equalizer.Bass += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Equalizer.Bass)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Equalizer.Bass);
			        			//strncat(s1, "                      ", 23 - FxMenu.Equalizer.Bass);
			        			print_two(MenuEqParam[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{ 
		        				FxMenu.Equalizer.Param = EQ_TYPE_OFF;
	        					print_two(MenuEffect[FxMenu.Effect-1], "                      ");
		            		}
		        		}
	        		}
	        	}
	        	else if (FxMenu.Effect == PHASER) 
	        	{
	        		if (FxMenu.Phaser.Param == PHASER_PARAM_OFF)
	        		{ 
	                	if (LcdMsg == LCD_MSG_BUTTON_L)
	                	{
		        			FxMenu.Phaser.Param += 1;
		        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                      ");
	                	}
	            		else if (LcdMsg == LCD_MSG_BUTTON_R)
	            		{ 
	        				FxMenu.Effect = EFFECT_OFF;	// This is for last effect in FxMenu
			        		print_two(MenuEffect[FxMenu.Effect], "                ");
	            		}
	        		}
	        		else if (FxMenu.Phaser.Param == RATE)
	        		{
		        		if (FxMenu.Phaser.Rate == 0)
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Phaser.Rate += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Phaser.Rate)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Phaser.Rate);
			        			//strncat(s1, "                      ", 23 - FxMenu.Phaser.Rate);
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{
		        				FxMenu.Phaser.Param = DEPTH;
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Phaser.Rate += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Phaser.Rate)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Phaser.Rate);
			        			//strncat(s1, "                      ", 23 - FxMenu.Phaser.Rate);
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{
		        				FxMenu.Phaser.Param = DEPTH;
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                      ");
		            		}
		        		}
	        		}
	        		else if (FxMenu.Phaser.Param == DEPTH)
	        		{
		        		if (FxMenu.Phaser.Depth == 0)
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Phaser.Depth += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Phaser.Depth)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Phaser.Depth);
			        			//strncat(s1, "                      ", 23 - FxMenu.Phaser.Depth);
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{
		        				FxMenu.Phaser.Param = RESONANCE;
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Phaser.Depth += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Phaser.Depth)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Phaser.Depth);
			        			//strncat(s1, "                      ", 23 - FxMenu.Phaser.Depth);
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{
		        				FxMenu.Phaser.Param = RESONANCE;
			        			print_two(MenuPhaser[FxMenu.Phaser.Param-1], "                      ");
		            		}
		        		}
	        		}
	        		else if (FxMenu.Phaser.Param == RESONANCE)
	        		{
		        		if (FxMenu.Phaser.Resonance == 0)
		        		{ 
		                	if (LcdMsg == LCD_MSG_BUTTON_L)
		                	{
			        			FxMenu.Phaser.Resonance += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Phaser.Depth)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Phaser.Resonance);
			        			//strncat(s1, "                      ", 23 - FxMenu.Phaser.Resonance);
			        			print_two(MenuPhaser[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_R)
		            		{ 
		        				FxMenu.Phaser.Param = PHASER_PARAM_OFF;
	        					print_two(MenuEffect[FxMenu.Effect-1], "                      ");
		            		}
		        		}
		        		else
		        		{
		                	if (LcdMsg == LCD_MSG_BUTTON_R)
		                	{
			        			FxMenu.Phaser.Resonance += 1;
			        			for(i=0; i < 23; i++) {
			        				if(i < FxMenu.Phaser.Depth)
			        					s1[i] = MenuValString[i];
			        				else 
			        					s1[i] = ' ';
			        			} 
			        			//strncpy(s1, MenuValString, FxMenu.Phaser.Resonance);
			        			//strncat(s1, "                      ", 23 - FxMenu.Phaser.Resonance);
			        			print_two(MenuPhaser[FxMenu.Equalizer.Param-1], s1);
		                	}
		            		else if (LcdMsg == LCD_MSG_BUTTON_L)
		            		{ 
		        				FxMenu.Phaser.Param = PHASER_PARAM_OFF;
	        					print_two(MenuEffect[FxMenu.Effect-1], "                      ");
		            		}
		        		}
	        		}
	        	}
	        }
        }
    }

	return 0;
}
#endif

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 print_playaudio()                                                 *
 *                                                                          *
 *      Print "   Play Audio"                            				    *
 *            "      Play Audio"                                            *
 * ------------------------------------------------------------------------ */
Int16 print_playaudio()
{
	char ti[23]; 
		
	OSD9616_send(0x00,0x2e); // Turn off scrolling 
	clear_lcd();
	sprintf(ti,"                       "); //clear	
	    /* Write to page 1 */
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
   	
   	sprintf(ti,"  Play Audio");
	printstring(ti);
	    /* Write to page 0*/ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
	sprintf(ti,"  From Your PC");
	printstring(ti);
	
	OSD9616_send(0x00,0x2f); // Turn on scrolling 
	
	return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 print_two(s1, s2)                                                 *
 *                                                                          *
 *      Print s1				                            				*
 *            s2                                                            *
 * ------------------------------------------------------------------------ */
Int16 print_two(char *s1, char *s2)
{
	OSD9616_send(0x00,0x2e); // Turn off scrolling 
	clear_lcd();
	    /* Write to page 1 */
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
   	
	printstring(s1);
	    /* Write to page 0*/ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
	printstring(s2);
	
	//OSD9616_send(0x00,0x2f); // Turn on scrolling 
	
	return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 print_charROM()                                                       *
 *                                                                          *
 *      Print "Industry lowest"                            				*
 *            "power dsp C5515"                                                             *
 * ------------------------------------------------------------------------ */
Int16 print_charROM()
{
	char crom[16]; 
	Uint16 i,j;
	
	OSD9616_send(0x00,0x2e); // Turn off scrolling 
	clear_lcd();
	
	for (j=0; j<8; j++) {
	    /* Write to page 1 */
    	OSD9616_send(0x00,0x00);   // Set low column address
    	OSD9616_send(0x00,0x10);   // Set high column address
    	OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
   		for (i=0; i<16; i++) {
   			crom[i] = i + j*32;
   		}
		printstring(crom);
	 
	    /* Write to page 0*/ 
    	OSD9616_send(0x00,0x00);   // Set low column address
    	OSD9616_send(0x00,0x10);   // Set high column address
    	OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
   		for (i=0; i<16; i++) {
   			crom[i] = i + 16 + j*32;
   		}	
		printstring(crom);
		//USBSTK5515_waitusec(3000000);
	}
	//OSD9616_send(0x00,0x2f); // Turn on scrolling 
	return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Int16 oled_init()                                                       *
 *                                                                          *
 *      Testing function for the OSD9616 display                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 oled_init()
{
	Int16 i2c_err;
	Uint16 cmd[10];    // For multibyte commands
	
	
	/* Initialize I2C */
    //USBSTK5515_I2C_init( );
    
    /* Initialize LCD power */
    //USBSTK5515_GPIO_setDirection( 12, 1 );  // Output
    //USBSTK5515_GPIO_setOutput( 12, 1 );     // Enable 13V 
    
    /* Initialize OSD9616 display */
    OSD9616_send(0x00,0x2e); // Turn off scrolling 
    
    i2c_err = OSD9616_send(0x00,0x00); // Set low column address
    i2c_err = OSD9616_send(0x00,0x10); // Set high column address
    if(i2c_err)  // Don't setup display if not connected
        return 1;
    
    OSD9616_send(0x00,0x40); // Set start line address

    cmd[0] = 0x00 & 0x00FF;  // Set contrast control register
    cmd[1] = 0x81;
    cmd[2] = 0x7f;
    //USBSTK5515_I2C_write( OSD9616_I2C_ADDR, cmd, 3 );
	I2C_Write(hi2c, OSD9616_I2C_ADDR, 3, cmd);
	
    OSD9616_send(0x00,0xa1); // Set segment re-map 95 to 0
    OSD9616_send(0x00,0xa6); // Set normal display

    cmd[0] = 0x00 & 0x00FF;  // Set multiplex ratio(1 to 16)
    cmd[1] = 0xa8; 
    cmd[2] = 0x0f;
    //USBSTK5515_I2C_write( OSD9616_I2C_ADDR, cmd, 3 );
    I2C_Write(hi2c, OSD9616_I2C_ADDR, 3, cmd);

    OSD9616_send(0x00,0xd3); // Set display offset
    OSD9616_send(0x00,0x00); // Not offset
    OSD9616_send(0x00,0xd5); // Set display clock divide ratio/oscillator frequency
    OSD9616_send(0x00,0xf0); // Set divide ratio

    cmd[0] = 0x00 & 0x00FF;  // Set pre-charge period
    cmd[1] = 0xd9;
    cmd[2] = 0x22;
    I2C_Write(hi2c, OSD9616_I2C_ADDR, 3, cmd);

    cmd[0] = 0x00 & 0x00FF;  // Set com pins hardware configuration
    cmd[1] = 0xda;
    cmd[2] = 0x02;
    I2C_Write(hi2c, OSD9616_I2C_ADDR, 3, cmd);

    OSD9616_send(0x00,0xdb); // Set vcomh
    OSD9616_send(0x00,0x49); // 0.83*vref
    
    cmd[0] = 0x00 & 0x00FF;  //--set DC-DC enable
    cmd[1] = 0x8d;
    cmd[2] = 0x14;
    I2C_Write(hi2c, OSD9616_I2C_ADDR, 3, cmd);
	
    OSD9616_send(0x00,0xaf); // Turn on oled panel    
    
	clear_lcd();
	
#if 0    
    /* Set vertical and horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x29;  // Vertical and Right Horizontal Scroll
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x03;  // Set time interval between each scroll step
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x01;  // Vertical scrolling offset
    OSD9616_multiSend( cmd, 7 );
    OSD9616_send(0x00,0x2f);
    /* Keep first 8 rows from vertical scrolling  */
    cmd[0] = 0x00;
    cmd[1] = 0xa3;  // Set Vertical Scroll Area
    cmd[2] = 0x08;  // Set No. of rows in top fixed area
    cmd[3] = 0x08;  // Set No. of rows in scroll area
    OSD9616_multiSend( cmd, 4 );
#else
    cmd[0] = 0x00;
    cmd[1] = 0x27;  // Horizontal Scroll
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x02;  // Set time interval between each scroll step
    cmd[5] = 0x02;  // Define end page address
    cmd[6] = 0x00;  // Dummy
    cmd[7] = 0xFF;  // Dummy
    //OSD9616_multiSend( cmd, 8 );
    OSD9616_multiSend( cmd, 8, hi2c); 
    OSD9616_send(0x00,0x2f);
    
#endif
	return 0;
}

/*****************************************************************************/
/* amplitude2bargraph()                                                      */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Parameter 1:  Input value in range -32767 to - 32768.                     */
/*                                                                           */
/* RETURNS: Bargraph pixel value between 0 and FFFFh.                        */
/*                                                                           */
/*****************************************************************************/
#if 0
static unsigned int amplitude2bargraph(int input)
{
 if ( input < 0 ) 
  {
    input = -input;
  }

 if ( input > 23166)
  {
    return (0xFFFF); /* All LEDs on */
  }
 else if (input > 16384)
  {
    return (0xFFFF);
  }
 else if ( input > 11583)
  {
    return (0xFFFF);
  }
 else if ( input > 8192)
  {
    return (0xFFFF);
  }
 else if ( input > 5791)
  {
    return (0xFFFF);
  }
 else if ( input > 4096)
  {
    return (0xFFFF);
  }
 else if (input > 2896)
  {
    return ( 0xFFFF);
  }
 else if ( input > 2048)
  {
    return (0xFFFF);
  }
 else if ( input > 1448)
  {
   return (0xFFFF);
  }
 else if ( input > 1024)
  {
   return (0xFFFE );
  }
 else if ( input > 724)
  {
   return (0xFFFC);
  }
 else if ( input > 512)
  {
   return (0xFFF8);
  }
 else if ( input > 362)
  {
   return (0xFFF0);
  }
 else if ( input > 256)
  {
   return (0xFFE0);
  }
 else if ( input > 181)
  {
   return (0xFFC0);
  }
 else if ( input > 128)
  {
   return (0xFF80);
  }
 else if ( input > 90)
  {
   return (0xFF00);
  } 
 else if ( input > 64)
  {
   return (0xFE00);
  } 
 else if ( input > 45)
  {
   return (0xFC00);
  } 
 else if ( input > 32)
  {
   return (0xF800);
  }  
 else if (input > 22)
  {
   return (0xF000);
  } 
 else if ( input > 16)
  {
   return(0xE000);
  } 
 else if ( input > 11)
  {
   return (0xC000);
  } 
 else if ( input > 8)
  {
   return (0x8000);
  } 
 else
  {
    return (0);
  }
}
#else
static unsigned int amplitude2bargraph(int input)
{
 if ( input < 0 ) 
  {
    input = -input;
  }

 if ( input > 23166)
  {
    return (0xFFFF); /* All LEDs on */
  }
 else if (input > 16384)
  {
    return (0xFFFF);
  }
 else if ( input > 11583)
  {
    return (0xFFFF);
  }
 else if ( input > 8192)
  {
    return (0xFFFF);
  }
 else if ( input > 5791)
  {
    return (0xFFFF);
  }
 else if ( input > 4096)
  {
    return (0xFFFF);
  }
 else if (input > 2896)
  {
    return ( 0xFFFF);
  }
 else if ( input > 2048)
  {
    return (0xFFFF);
  }
 else if ( input > 1448)
  {
   return (0xFFFF);
  }
 else if ( input > 1024)
  {
   return (0x7FFF );
  }
 else if ( input > 724)
  {
   return (0x3FFF);
  }
 else if ( input > 512)
  {
   return (0x1FFF);
  }
 else if ( input > 362)
  {
   return (0x0FFF);
  }
 else if ( input > 256)
  {
   return (0x07FF);
  }
 else if ( input > 181)
  {
   return (0x03FF);
  }
 else if ( input > 128)
  {
   return (0x01FF);
  }
 else if ( input > 90)
  {
   return (0x00FF);
  } 
 else if ( input > 64)
  {
   return (0x007F);
  } 
 else if ( input > 45)
  {
   return (0x003F);
  } 
 else if ( input > 32)
  {
   return (0x001F);
  }  
 else if (input > 22)
  {
   return (0x000F);
  } 
 else if ( input > 16)
  {
   return(0x0007);
  } 
 else if ( input > 11)
  {
   return (0x0003);
  } 
 else if ( input > 8)
  {
   return (0x0001);
  } 
 else
  {
    return (0);
  }
}
#endif

/*****************************************************************************/
/* oled_display_bargraph()                                                   */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Parameter 1:  Pointer to array containing 96 16-bit input values.         */
/*                                                                           */
/* RETURNS: Nothing                                                          */
/*                                                                           */
/*****************************************************************************/
int oled_display_bargraph (int * message)
{
    int i;
    Uint16 input;
    Uint16 pixels;	

	OSD9616_send(0x00,0x2E);           // Turn off scrolling
	
    /* Fill page 0 */ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5

    /* Send topline from high to low */
    for ( i = 0; i <96/2; i++)
     {
       input = amplitude2bargraph(message[i]);       
     
       pixels = (input&0xFF); /* Extract low byte */

       OSD9616_send(0x40,pixels);
       OSD9616_send(0x40,pixels);
     }
    
    /* Write to page 1*/ 
    OSD9616_send(0x00,0x00);   // Set low column address
    OSD9616_send(0x00,0x10);   // Set high column address
    OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
   

  /* Send bottomline */
    for ( i = 0 ; i < 96/2; i++ )
     {
       input = amplitude2bargraph(message[i]);       
     
       pixels = (input>>8 ); /* Extract high byte */
       
       OSD9616_send(0x40,pixels);
       OSD9616_send(0x40,pixels);
     }

	return 0;
}
#endif

