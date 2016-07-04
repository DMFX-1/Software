/************************************************************************
 *  @file lcdST7565.c
 *
 *  @brief
 *
 *   Interface program for ST7656 Graphic LCD 128x64 pixels B/W
 *  ------------------------------------------------------------
 *
 *  This program help to initialize, write and draw graphic on this
 *  LCD
 *
 *  White LED ST7565 LCD
 *
 *  The LCD uses the SPI0 interface and GP12 to control LCD_A0
 *
 *  Font generation is done using the MikroElektronica GLCD Font Creator:
 *  http://www.mikroe.com/glcd-font-creator/
 *
 *  Author: Diego de la Cruz
 *  Created on: Nov 8, 2015
 *  @note
 *  Adapted from boseji <prog.ic@live.in>
 *
 *  @license
 *  License:
 *  This work is licensed under a Creative Commons
 *  Attribution-ShareAlike 4.0 International License.
 *  http://creativecommons.org/licenses/by-sa/4.0/
 *
 ************************************************************************/

/************************************************************************/
/* Includes                                                             */
/************************************************************************/
#include "csl_ioport.h"
#include "csl_spi.h"
#include "csl_general.h"
#include "csl_sysctrl.h"
#include <pal_osWait.h>
#include "DMFX2_CSL_BIOS_cfg.h"
#include "lcdST7565.h"
#include "string.h"
#include <csl_sar_intc.h>
#include <csl_intc.h>
#include "csl_gpio_intc.h"
#include "DMFX_debug.h"
#include "tuner.h"
#include "dsp_fx.h"

/************************************************************************/
/* ST7565 LCD Connection								                */
/************************************************************************/

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/
extern Uint16       SarData[];        // SAR Read Buffer
extern Uint16 		NavigatorButton;  // Stores last Navigator button pressed
extern Uint16 		chanNo;			  // SAR Channel number 0-3 or 2-3
extern CSL_SarHandleObj *SarHandle;   // SAR handle
extern volatile char	NoiseSup_on;
extern Uint16 FxCmd[FXCMD_SIZE];

/* Global position storage for the Cursor */
static Uint16 gw_row = 0;
static Uint16 gw_column = 0;

ContextMenu_t *LcdCtx = &MenuTop[0]; 	// saves LCD Menu context

char InitMenu [4][16] = {
		    "    Rezzonics   ",
		    "      DMFX-1    ",
		    "       v1.0     ",
		    "       2015     "
};

//------------------------------------------------------------------------------
// LCD Menus
//------------------------------------------------------------------------------
ContextMenu_t MenuTop [15] =
{
	{"1  Main settings", NULL, &MenuMainSet[0],  &MenuTop[1],  &MenuTop[14], MAIN_SETTINGS, 1},
    {"2  Patch select ", NULL, &MenuPatchSet[0], &MenuTop[2],  &MenuTop[0],  PATCH_MAIN, 	1},
    {"3  Compressor   ", NULL, &MenuComp[0],     &MenuTop[3],  &MenuTop[1],  COMPRESSOR, 	1},
    {"4  Distortion   ", NULL, &MenuDist[0],     &MenuTop[4],  &MenuTop[2],  DISTORTION, 	1},
    {"5  Equalizer    ", NULL, &MenuEqParam[0],  &MenuTop[5],  &MenuTop[3],  EQUALIZER, 	1},
    {"6  Phaser       ", NULL, &MenuPhaser[0],   &MenuTop[6],  &MenuTop[4],  PHASER, 		1},
    {"7  Noise Cancel ", NULL, &MenuNoise[0],    &MenuTop[7],  &MenuTop[5],  NOISE_CANCEL, 	1},
    {"8  Wah-Wah      ", NULL, &MenuWahType[0],  &MenuTop[8],  &MenuTop[6],  WAH_WAH, 		1},
    {"9  Delay        ", NULL, &MenuDelay[0],    &MenuTop[9],  &MenuTop[7],  DELAY, 		1},
    {"10 Echo         ", NULL, &MenuEcho[0],     &MenuTop[10], &MenuTop[8],  ECHO, 			1},
    {"11 Chorus       ", NULL, &MenuChorus[0],   &MenuTop[11], &MenuTop[9],  CHORUS, 		1},
    {"12 Flanger      ", NULL, &MenuFlanger[0],  &MenuTop[12], &MenuTop[10], FLANGER, 		1},
    {"13 Pitch Shift  ", NULL, &MenuPhShift[0],  &MenuTop[13], &MenuTop[11], PITCH_SHIFT, 	1},
    {"14 Tremolo      ", NULL, &MenuTremolo[0],  &MenuTop[14], &MenuTop[12], TREMOLO, 		1},
    {"15 Reverb       ", NULL, &MenuReverb[0],   &MenuTop[0],  &MenuTop[13], REVERB, 		1}
};

ContextMenu_t MenuMainSet [6] =
{
    {"1  Bypass       ", &MenuTop[0], NULL, 		&MenuMainSet[1], &MenuMainSet[5], BYPASS, 1}, 		//=> Bypass 0/1
    {"2  LineOut Vol  ", &MenuTop[0], &MenuPot[0],  &MenuMainSet[2], &MenuMainSet[0], LOUT_VOL, 0}, 	//=> Main Volume: Pot 0-1000
    {"3  HeadPh Vol   ", &MenuTop[0], &MenuPot[0],  &MenuMainSet[3], &MenuMainSet[1], HP_VOL, 0}, 	    //=> Main Volume: Pot 0-1000
    {"4  Analog Gain  ", &MenuTop[0], &MenuPot[0],  &MenuMainSet[4], &MenuMainSet[2], ANA_GAIN, 24}, 	//=> Main Volume: Pot 0-1000
    {"5  Tuner        ", &MenuTop[0], &MenuTuner[0],&MenuMainSet[5], &MenuMainSet[3], TUNER, 1},  		//=> Sampling frequency: 48, 32, 16...
    {"6  Sampling Freq", &MenuTop[0], NULL, 		&MenuMainSet[0], &MenuMainSet[4], SAMPLE_FREQ, 1}  	//=> Sampling frequency: 48, 32, 16...
};

ContextMenu_t MenuPatchSet [3] = {
	{"1  Patch       ", &MenuTop[1], NULL, &MenuPatchSet[1], &MenuPatchSet[2], PATCH_1, 1},  //=> Patch 0: points to a full FX structure
	{"2  Patch       ", &MenuTop[1], NULL, &MenuPatchSet[2], &MenuPatchSet[0], PATCH_2, 2},  //=> Patch 1: points to a full FX structure
	{"3  Patch       ", &MenuTop[1], NULL, &MenuPatchSet[0], &MenuPatchSet[1], PATCH_3, 3}   //=> Patch 2: points to a full FX structure
	//=> 0x1F Leave space for up to 8 patches
};

ContextMenu_t MenuPatchCmd [2] = {
	{"1  Record      ", &MenuPatchSet[0], NULL, &MenuPatchCmd[1], &MenuPatchCmd[1], PATCH_REC, 1},  //=> Patch 0: points to a full FX structure
	{"2  Select      ", &MenuPatchSet[0], NULL, &MenuPatchCmd[0], &MenuPatchCmd[0], PATCH_SEL, 1},  //=> Patch 1: points to a full FX structure
	//=> 0x1F Leave space for up to 8 patches
};

ContextMenu_t MenuComp [4] = {
	{"1  Sustain      ", &MenuTop[2], &MenuPot[0], &MenuComp[1], &MenuComp[3], CMP_SUSTAIN, 255},  	//=> Sustain: Potvalue 0-255
	{"2  Attack       ", &MenuTop[2], &MenuPot[0], &MenuComp[2], &MenuComp[0], CMP_ATTACK, 10},  	//=> Attack:  Potvalue 0-255
	{"3  Decay        ", &MenuTop[2], &MenuPot[0], &MenuComp[3], &MenuComp[1], CMP_DECAY, 250},  	//=> Decay:  Potvalue 0-255
	{"4  Level        ", &MenuTop[2], &MenuPot[0], &MenuComp[0], &MenuComp[2], CMP_LEVEL, 255}   	//=> Level:   Potvalue 0-255
};

ContextMenu_t MenuDist [4] = {
	{"1  Type         ", &MenuTop[3], &MenuDistType[0], &MenuDist[1], &MenuDist[3], DIST_TYPE, 1},
	{"2  Drive        ", &MenuTop[3], &MenuPot[0], 	    &MenuDist[2], &MenuDist[0], DIST_DRIVE, 255},  	//=> Drive:  Potvalue 0-255
	{"3  Tone         ", &MenuTop[3], &MenuPot[0],  	&MenuDist[3], &MenuDist[1], DIST_TONE, 255},  	//=> Tone:   Potvalue 0-255
	{"4  Level        ", &MenuTop[3], &MenuPot[0], 		&MenuDist[0], &MenuDist[2], DIST_LEVEL, 255}   	//=> Level:  Potvalue 0-255
};

ContextMenu_t MenuDistType [8] = {
	{"1  Vintage      ", &MenuDist[0], NULL, 			&MenuDistType[1], &MenuDistType[7], DIST_TYPE_LIST, 1},  //=> Vintage 1
	{"2  Warm         ", &MenuDist[0], NULL, 			&MenuDistType[2], &MenuDistType[0], DIST_TYPE_LIST, 2},  //=> Warm    2
	{"3  Crunch       ", &MenuDist[0], NULL, 			&MenuDistType[3], &MenuDistType[1], DIST_TYPE_LIST, 3},  //=> Crunch  3
	{"4  Blues        ", &MenuDist[0], NULL, 			&MenuDistType[4], &MenuDistType[2], DIST_TYPE_LIST, 4},  //=> Blues   4
	{"5  Hard         ", &MenuDist[0], NULL, 			&MenuDistType[5], &MenuDistType[3], DIST_TYPE_LIST, 5},  //=> Hard    5
	{"6  Metal        ", &MenuDist[0], NULL, 			&MenuDistType[6], &MenuDistType[4], DIST_TYPE_LIST, 6},  //=> Metal   6
	{"7  Fuzz         ", &MenuDist[0], NULL, 			&MenuDistType[7], &MenuDistType[5], DIST_TYPE_LIST, 7},  //=> Fuzz    7
	{"8  General      ", &MenuDist[0], &MenuDistGen[0], &MenuDistType[0], &MenuDistType[6], DIST_TYPE_LIST, 8}   //=> General 8
};

ContextMenu_t MenuDistGen [4] = {
	{"1  Drive 8dB    ", &MenuDistType[7], NULL, &MenuDistGen[1], &MenuDistGen[3], DRV1_8DB, 1},   //=> Drive 8dB 	 0-1
	{"2  Drive 16dB   ", &MenuDistType[7], NULL, &MenuDistGen[2], &MenuDistGen[0], DRV0_16DB, 1},  //=> Drive 16dB   0-1
	{"3  Mode 0 Mids  ", &MenuDistType[7], NULL, &MenuDistGen[3], &MenuDistGen[1], MD0_MIDS, 1},   //=> Mode 0 Mids  0-1
	{"4  Mode 1 Clip  ", &MenuDistType[7], NULL, &MenuDistGen[0], &MenuDistGen[2], MD1_CLIP, 1},   //=> Mode 1 Clip  0-1
};

ContextMenu_t MenuEqParam [4] = {
	{"1  Level        ",  &MenuTop[4], &MenuPot[0], &MenuEqParam[1], &MenuEqParam[3], EQ_LEVEL, 9},  //=> Level:  Potvalue 0-255
	{"2  Bass         ",  &MenuTop[4], &MenuPot[0], &MenuEqParam[2], &MenuEqParam[0], EQ_BASS, 5},   //=> Bass:   Potvalue 0-255
	{"3  Middle       ",  &MenuTop[4], &MenuPot[0], &MenuEqParam[3], &MenuEqParam[1], EQ_MIDDLE, 5}, //=> Middle: Potvalue 0-255
	{"4  Treble       ",  &MenuTop[4], &MenuPot[0], &MenuEqParam[0], &MenuEqParam[2], EQ_TREBLE, 5}  //=> Treble: Potvalue 0-255
};

ContextMenu_t MenuPhaser [4] = {
	{"1  Rate         ",  &MenuTop[5], &MenuPot[0], &MenuPhaser[1], &MenuPhaser[3], PH_RATE, 25},			//=> Rate:   	  Potvalue 0-255 100*rate(dHz)/255
	{"2  Depth        ",  &MenuTop[5], &MenuPot[0], &MenuPhaser[2], &MenuPhaser[0], PH_DEPTH, 255},			//=> Depth:   	  Potvalue 0-255
	{"3  Resonance    ",  &MenuTop[5], &MenuPot[0], &MenuPhaser[3], &MenuPhaser[1], PH_RESONANCE, 255},		//=> Resonance:   Potvalue 0-255
	{"4  Mono/Stereo  ",  &MenuTop[5], &MenuPh_Stereo[0], &MenuPhaser[0], &MenuPhaser[2], PH_STEREO, 1}	    //=> Stereo:   	  Default Mono
};

ContextMenu_t MenuPh_Stereo [3] = {
	{"1  Mono        ", &MenuPhaser[3], NULL,  &MenuPh_Stereo[1], &MenuPh_Stereo[2], PH_STEREO_LIST, 1},  //=> Mono 	1
	{"2  Stereo      ", &MenuPhaser[3], NULL,  &MenuPh_Stereo[2], &MenuPh_Stereo[0], PH_STEREO_LIST, 2},  //=> Stereo	2
	{"3  Leslie      ", &MenuPhaser[3], NULL,  &MenuPh_Stereo[0], &MenuPh_Stereo[1], PH_STEREO_LIST, 3},  //=> Leslie 	3
};

ContextMenu_t MenuNoise [2] = {
	{"1  Threshold    ", &MenuTop[6], &MenuPot[0], &MenuNoise[1], &MenuNoise[1], NS_THRESHOLD, 100},	//=> Threshold:   Potvalue 0-255
	{"2  Auto         ", &MenuTop[6], NULL, 	   &MenuNoise[0], &MenuNoise[0], NS_AUTO, 0}			//=> Auto:   	  Potvalue 0-255
};

ContextMenu_t MenuWahType[2] = {
	{"1  Wah-wah A   ",  &MenuTop[7], &MenuPot[0],   &MenuWahType[1], &MenuWahType[1], WAH_A, 255},  //=> WahFreq:   Potvalue 0-255
	{"2  Wah-wah B   ",  &MenuTop[7], &MenuPot[0],   &MenuWahType[0], &MenuWahType[0], WAH_B, 255}   //=> WahFreq:   Potvalue 0-255
};

ContextMenu_t MenuDelay [9] = {
	{"1  Type         ", &MenuTop[8], &MenuDelType[0],  &MenuDelay[1], &MenuDelay[8], DEL_TYPE, 1},
	{"2  Delay        ", &MenuTop[8], &MenuPot[0], 		&MenuDelay[2], &MenuDelay[0], DEL_DELAY, 94},  		//=> Delay:   	  Potvalue 0-255 delay/375(s)
	{"3  Rate         ", &MenuTop[8], &MenuPot[0], 		&MenuDelay[3], &MenuDelay[1], DEL_RATE, 25},  		//=> Rate:   	  Potvalue 0-255
	{"4  Depth        ", &MenuTop[8], &MenuPot[0], 		&MenuDelay[4], &MenuDelay[2], DEL_DEPTH, 255}, 		//=> Depth:   	  Potvalue 0-255
	{"5  Feedback     ", &MenuTop[8], &MenuPot[0], 		&MenuDelay[5], &MenuDelay[3], DEL_FB, 255},   		//=> FeedBack:    Potvalue 0-255
	{"6  FeedFwd      ", &MenuTop[8], &MenuPot[0], 		&MenuDelay[6], &MenuDelay[4], DEL_FF, 255},   		//=> FeedFwd:  	  Potvalue 0-255
	{"7  Blend        ", &MenuTop[8], &MenuPot[0], 		&MenuDelay[7], &MenuDelay[5], DEL_BL, 255},   		//=> Blend:   	  Potvalue 0-255
	{"8  LFO          ", &MenuTop[8], &MenuDel_LFO[0], 	&MenuDelay[8], &MenuDelay[6], DEL_LFO, 1},   		//=> LFO:   	  Potvalue 0-255
	{"9  Stereo       ", &MenuTop[8], &MenuDel_Stereo[0], &MenuDelay[0], &MenuDelay[7], DEL_STEREO, 1} 		//=> Stereo:   	  Default Mono
};

ContextMenu_t MenuDelType [4] = {
	{"1  Normal      ", &MenuDelay[0], NULL,  &MenuDelType[1], &MenuDelType[3], DEL_TYPE_LIST, 1},  //=> Normal 	1
	{"2  Pan         ", &MenuDelay[0], NULL,  &MenuDelType[2], &MenuDelType[0], DEL_TYPE_LIST, 2},  //=> Pan 		2
	{"3  Echo        ", &MenuDelay[0], NULL,  &MenuDelType[3], &MenuDelType[1], DEL_TYPE_LIST, 3},  //=> Echo	 	3
	{"4  Tap         ", &MenuDelay[0], NULL,  &MenuDelType[0], &MenuDelType[2], DEL_TYPE_LIST, 4}   //=> Tap 		4
};

ContextMenu_t MenuDel_LFO [4] = {
	{"1  Sine        ", &MenuDelay[7], NULL,  &MenuDel_LFO[1], &MenuDel_LFO[3], DEL_LFO_LIST, 1},  //=> Sine 	 1
	{"2  Noise       ", &MenuDelay[7], NULL,  &MenuDel_LFO[2], &MenuDel_LFO[0], DEL_LFO_LIST, 2},  //=> Noise 	 2
	{"3  Triangle    ", &MenuDelay[7], NULL,  &MenuDel_LFO[3], &MenuDel_LFO[1], DEL_LFO_LIST, 3},  //=> Triangle 3
	{"4  Sawtooth    ", &MenuDelay[7], NULL,  &MenuDel_LFO[0], &MenuDel_LFO[2], DEL_LFO_LIST, 4}   //=> Sawtooth 4
};

ContextMenu_t MenuDel_Stereo [3] = {
	{"1  Mono        ", &MenuDelay[8], NULL,  &MenuDel_Stereo[1], &MenuDel_Stereo[2], DEL_STEREO_LIST, 1},  //=> Mono 	1
	{"2  Stereo      ", &MenuDelay[8], NULL,  &MenuDel_Stereo[2], &MenuDel_Stereo[0], DEL_STEREO_LIST, 2},  //=> Stereo	2
	{"3  Leslie      ", &MenuDelay[8], NULL,  &MenuDel_Stereo[0], &MenuDel_Stereo[1], DEL_STEREO_LIST, 3}   //=> Leslie 3
};

ContextMenu_t MenuEcho [5] = {
	{"1  Delay        ", &MenuTop[9], &MenuPot[0], 			&MenuEcho[1], &MenuEcho[4], ECHO_DELAY, 94},  		//=> Echo:   	  Potvalue 0-255
	{"2  Depth        ", &MenuTop[9], &MenuPot[0], 			&MenuEcho[2], &MenuEcho[0], ECHO_DEPTH, 255}, 		//=> Depth:   	  Potvalue 0-255
	{"3  Feedback     ", &MenuTop[9], &MenuPot[0], 			&MenuEcho[3], &MenuEcho[1], ECHO_FB, 0},   			//=> FeedBack:    Potvalue 0-255
	{"4  FeedFwd      ", &MenuTop[9], &MenuPot[0], 			&MenuEcho[4], &MenuEcho[2], ECHO_FF, 255},   		//=> FeedFwd:  	  Potvalue 0-255
	{"5  Stereo       ", &MenuTop[9], &MenuEcho_Stereo[0], 	&MenuEcho[0], &MenuEcho[3], ECHO_STEREO, 1} 		//=> Stereo:   	  Default Mono
};

ContextMenu_t MenuEcho_Stereo [3] = {
	{"1  Mono        ", &MenuEcho[4], NULL,  &MenuEcho_Stereo[1], &MenuEcho_Stereo[2], ECHO_STEREO_LIST, 1},  //=> Mono 	1
	{"2  Stereo      ", &MenuEcho[4], NULL,  &MenuEcho_Stereo[2], &MenuEcho_Stereo[0], ECHO_STEREO_LIST, 2},  //=> Stereo	2
	{"3  Leslie      ", &MenuEcho[4], NULL,  &MenuEcho_Stereo[0], &MenuEcho_Stereo[1], ECHO_STEREO_LIST, 3}   //=> Leslie 3
};

ContextMenu_t MenuChorus [5] = {
	{"1  Type         ", &MenuTop[10], &MenuChorusType[0], 	  &MenuChorus[1], &MenuChorus[4], CH_TYPE, 1},		//=> Type:   ISC, White
	{"2  Rate         ", &MenuTop[10], &MenuPot[0], 		  &MenuChorus[2], &MenuChorus[0], CH_RATE, 25},		//=> Rate:   Potvalue 0-255
	{"3  Depth        ", &MenuTop[10], &MenuPot[0], 		  &MenuChorus[3], &MenuChorus[1], CH_DEPTH, 255},	//=> Depth:  Potvalue 0-255
	{"4  Delay        ", &MenuTop[10], &MenuPot[0], 		  &MenuChorus[4], &MenuChorus[2], CH_DELAY, 255},  	//=> Echo:   	  Potvalue 0-255
	{"5  Stereo       ", &MenuTop[10], &MenuChorus_Stereo[0], &MenuChorus[0], &MenuChorus[3], CH_STEREO, 3}   	//=> Stereo:   	  Default Mono
};
ContextMenu_t MenuChorusType[2] = {
	{"1  Indus Std   ",  &MenuChorus[0], NULL,   &MenuChorusType[1], &MenuChorusType[1], CH_TYPE_LIST, 1},  	//=> Industrial Standard Chorus:   1
	{"2  White       ",  &MenuChorus[0], NULL,   &MenuChorusType[0], &MenuChorusType[0], CH_TYPE_LIST, 2}    	//=> White Chorus:  			   2
};

ContextMenu_t MenuChorus_Stereo [3] = {
	{"1  Mono        ", &MenuChorus[4], NULL,  &MenuChorus_Stereo[1], &MenuChorus_Stereo[2], CH_STEREO_LIST, 1},  //=> Mono 	1
	{"2  Stereo      ", &MenuChorus[4], NULL,  &MenuChorus_Stereo[2], &MenuChorus_Stereo[0], CH_STEREO_LIST, 2},  //=> Stereo	2
	{"3  Leslie      ", &MenuChorus[4], NULL,  &MenuChorus_Stereo[0], &MenuChorus_Stereo[1], CH_STEREO_LIST, 3}   //=> Leslie   3
};

ContextMenu_t MenuFlanger [6] = {
	{"1  Rate         ", &MenuTop[11], &MenuPot[0], 	  &MenuFlanger[1], &MenuFlanger[5], FL_RATE, 128},			//=> Rate:   	  Potvalue 0-255
	{"2  Depth        ", &MenuTop[11], &MenuPot[0], 	  &MenuFlanger[2], &MenuFlanger[0], FL_DEPTH, 255},			//=> Depth:   	  Potvalue 0-255
	{"3  Delay        ", &MenuTop[11], &MenuPot[0], 	  &MenuFlanger[3], &MenuFlanger[1], FL_DELAY, 255},			//=> Delay:   	  Potvalue 0-255
	{"4  Resonance    ", &MenuTop[11], &MenuPot[0], 	  &MenuFlanger[4], &MenuFlanger[2], FL_RESONANCE, 255},		//=> Resonance:   Potvalue 0-255
	{"5  Mono/Stereo  ", &MenuTop[11], &MenuFl_Stereo[0], &MenuFlanger[5], &MenuFlanger[3], FL_STEREO, 1},	    	//=> Stereo:   	  Default Mono
	{"6  Notches      ", &MenuTop[11], &MenuPot[0], 	  &MenuFlanger[0], &MenuFlanger[4], FL_NOTCHES, 12}	    	//=> Notches:     1 - 12
};
ContextMenu_t MenuFl_Stereo [3] = {
	{"1  Mono        ", &MenuFlanger[4], NULL,  &MenuFl_Stereo[1], &MenuFl_Stereo[2], FL_STEREO_LIST, 1},  //=> Mono 	1
	{"2  Stereo      ", &MenuFlanger[4], NULL,  &MenuFl_Stereo[2], &MenuFl_Stereo[0], FL_STEREO_LIST, 2},  //=> Stereo	2
	{"3  Leslie      ", &MenuFlanger[4], NULL,  &MenuFl_Stereo[0], &MenuFl_Stereo[1], FL_STEREO_LIST, 3},  //=> Leslie 	3
};
ContextMenu_t MenuPhShift [3] = {
	{"1  Pitch        ", &MenuTop[12], &MenuPot[0], &MenuPhShift[1], &MenuPhShift[1], PITCH_VAL, 6},	//=> Pitch:   	  Potvalue 0-6
	{"2  Mix          ", &MenuTop[12], &MenuPot[0], &MenuPhShift[0], &MenuPhShift[0], PS_MIX, 255},		//=> Mix  :  	  Potvalue 0-255
//	{"3  Effect level ", &MenuTop[12], &MenuPot[0], &MenuPhShift[0], &MenuPhShift[1], PS_FXLEVEL, 255}	//=> Level FX: 	  Potvalue 0-255
};
ContextMenu_t MenuTremolo [3] = {
	{"1  Rate         ", &MenuTop[13], &MenuPot[0], &MenuTremolo[1], &MenuTremolo[2], TR_RATE, 128},	//=> Rate:   	  Potvalue 0-255
	{"2  Depth        ", &MenuTop[13], &MenuPot[0], &MenuTremolo[2], &MenuTremolo[0], TR_DEPTH, 255},	//=> Depth:   	  Potvalue 0-255
	{"3  Delay        ", &MenuTop[13], &MenuPot[0], &MenuTremolo[0], &MenuTremolo[1], TR_DELAY, 255}	//=> Resonance:   Potvalue 0-255
};
ContextMenu_t MenuReverb [4] = {
	{"1  Delay        ", &MenuTop[14], &MenuPot[0], &MenuReverb[1], &MenuReverb[3], REV_DELAY, 255},  //=> Delay:  	  	Potvalue 0-255
	{"2  Dry Level    ", &MenuTop[14], &MenuPot[0], &MenuReverb[2], &MenuReverb[0], REV_DRY, 255}, 	  //=> Dry Level:	Potvalue 0-255
	{"3  Early Level  ", &MenuTop[14], &MenuPot[0], &MenuReverb[3], &MenuReverb[1], REV_EARLY, 255},  //=> Early Level: Potvalue 0-255
	{"4  Late Level   ", &MenuTop[14], &MenuPot[0], &MenuReverb[0], &MenuReverb[2], REV_LATE, 255}	  //=> Late Level:  Potvalue 0-255
};
#if 0
ContextMenu_t MenuRevType [4] = {
	{"1  Room        ", &MenuReverb[0], NULL,  &MenuRevType[1], &MenuRevType[3], REV_TYPE_LIST, 1},  //=> Room 	 1
	{"2  Hall        ", &MenuReverb[0], NULL,  &MenuRevType[2], &MenuRevType[0], REV_TYPE_LIST, 2},  //=> Hall 	 2
	{"3  Stadium     ", &MenuReverb[0], NULL,  &MenuRevType[3], &MenuRevType[1], REV_TYPE_LIST, 3},  //=> Stadium 	 3
	{"4  Cathedral   ", &MenuReverb[0], NULL,  &MenuRevType[0], &MenuRevType[2], REV_TYPE_LIST, 4}   //=> Cathedral 4
};
#endif

ContextMenu_t MenuTuner [1] = {
	{"", &MenuTop[0], NULL, &MenuTuner[0], &MenuTuner[0], 0x0, 255}
};

ContextMenu_t MenuPot [POTENTIOMETER] = {
#if (POTENTIOMETER == 1)
	{"", NULL, NULL, &MenuPot[0], &MenuPot[0], 0x0, 255}
#else
#if (POTENTIOMETER == 2)
	{"", NULL, NULL, &MenuPot[1], &MenuPot[1], 0x0, 255},
	{"", NULL, NULL, &MenuPot[0], &MenuPot[0], 0x0, 255}
#else
#if (POTENTIOMETER == 4)
	{"", NULL, NULL, &MenuPot[1], &MenuPot[3], 0x0, 255},
	{"", NULL, NULL, &MenuPot[2], &MenuPot[0], 0x0, 255},
	{"", NULL, NULL, &MenuPot[3], &MenuPot[1], 0x0, 255},
	{"", NULL, NULL, &MenuPot[0], &MenuPot[2], 0x0, 255}
#endif
#endif
#endif
};

char Notes[SCALE_SIZE+4][4] = {
    "    ", "    ", "C#1 ", "D1  ", "D#1 ",
	"E1  ", "F1  ", "F#1 ", "G1  ", "G#1 ",
	"A2  ", "A#2 ", "B2  ", "C2  ", "C#2 ",
	"D3  ", "D#3 ", "E3  ", "F3  ", "F#3 ",
	"G4  ", "G#4 ", "A4  ", "A#4 ",
	"B5  ", "C5  ", "C#5 ", "D5  ", "D#5 ",
	"e6  ", "f6  ", "f#6 ", "g6  ", "g#6 ",
	"a7  ", "a#7 ", "b7  ", "c7  ", "c#7 ",
	"d8  ", "d#8 ", "e8  ", "f8  ", "f#8 ",
	"g9  ", "g#9 ", "a9  ", "a#9 ",
	"b9  ", "c9  ",	"    ", "    "
};

/************************************************************************/
/* LCD Functions                                                        */
/************************************************************************/

/**
 *  Function to send in one byte Command to the LCD
 */
CSL_Status lcd_cmd(CSL_SpiHandle hSpi, Uint16 byte)
{
	CSL_Status result;

	byte &= 0xFF;
	set_LCD_A0(0);
	result = SPI_write(hSpi, &byte, 1);
	return (result);
}

/**
 *  Function to Send one byte of graphic data column (8 pixels) to LCD
 */
CSL_Status lcd_data(CSL_SpiHandle hSpi, Uint16 byte)
{
	CSL_Status result;

	byte &= 0xFF;
	set_LCD_A0(1);
	result = SPI_write(hSpi, &byte, 1);
	return (result);
}

/**
 *  Function to Reset the LCD to its initial state
 */
CSL_Status lcd_reset(CSL_SpiHandle hSpi)
{
	CSL_Status result;
	result = lcd_cmd(hSpi, ST7565_LCD_CMD_INTERNAL_RESET);
    return (result);
}
/**
 *  Function to control the Contrast and brightness ratio
 *  - This is generally a fixed value for a given LCD.
 */
CSL_Status lcd_bright (CSL_SpiHandle hSpi, Int16 bValue )
{
	CSL_Status result;
	result = lcd_cmd(hSpi, ST7565_LCD_CMD_SET_VOLUME_FIRST);
	if(result != 0) return result;
	return lcd_cmd(hSpi, ST7565_LCD_CMD_SET_VOLUME_SECOND | (bValue & 0x3f));
}
/**
 * @brief Function to position the draw cursor
 *    Need initialization of LCD @ref lcd_init before using this function
 *    This function also updates the cursor location
 *
 * @param bColumn The X axis location from the Top Right will be 0
 * @param bRow The Y axis location from the Bottom Left will be 0
 *
 * @return Status of the operation
 *        0 for successful operation
 *        -61 for Column error
 *        -62 for Row error
 */
CSL_Status lcd_goto (CSL_SpiHandle hSpi, Uint8 bColumn, Uint8 bRow )
{
  if (bColumn >= ST7565_LCD_MAX_COLUMNS)
  {
    return -61;
  }
  if (bRow >= (ST7565_LCD_MAX_ROWS))
  {
    return -62;
  }

  /* Get the Values into the Global position storage for the Cursor */
  gw_row = bRow;
  gw_column = bColumn;

  /* Set the LCD Row */
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_PAGE | (7 - bRow));

  /* Set the LCD Column */
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_COLUMN_LOWER | (bColumn & 0xf));
  /* 127 max */
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_COLUMN_UPPER | ((bColumn >> 4) & 0x7));
  return 0;
}
/**
 * @brief Function to Clear the Display graphic memory
 *    Need initialization of LCD @ref lcd_init before using this function
 *    This function also updates the cursor location
 *
 * @param None
 * @return Status of the Operation
 *      0 for successful operation
 *      Else the Status of the @ref lcd_goto in case of error
 */
CSL_Status lcd_clear ( CSL_SpiHandle hSpi )
{
  Uint8 r, c;
  int result = 0;

  /* Go through each page row of the display */
  for (r = 0; r < ST7565_LCD_MAX_ROWS ; r++)
  {
    result = lcd_goto(hSpi, 0, r); /* Position cursor */
    if(result != 0) return result;
    /* For each column, write out a blank (white) byte */
    for (c = 0; c < ST7565_LCD_MAX_COLUMNS ; c++)
    {
      lcd_data(hSpi, 0x00);
    }
  }
  /* Set the Final Address at the Top Left Corner */
  return lcd_goto(hSpi, 0, 0);
}

/**
 * @brief Function to Initialize the LCD driver with Reset
 * @details Before calling this we need to call @ref init_io
 *      - All previous status would be erased and display is refreshed
 *      - This needs to be done before writing/drawing any thing to the LCD
 *
 * @param None
 * @return Status code for the Operaiton
 *        0 for successful operation
 *        -24 and -25 for error in I/O operations
 *        Else the last operation @ref lcd_clear
 */
CSL_Status lcd_init(CSL_SpiHandle hSpi)
{
	Int16 			result;
	SPI_Config		hwConfig;

	result = SPI_init();
	if(CSL_SOK != result)
	{
		result = 1;
		return (result);
	}
	else
	{
		LOG_printf(&trace, "SPI Instance Initialize successfully");
	}

	hSpi = SPI_open(SPI_CS_NUM_1, SPI_POLLING_MODE);
	if(NULL == hSpi)
	{
		return (1);
	}
	else
	{
		LOG_printf(&trace, "SPI 1 Instance Opened successfully\n");
	}
	/** Set the hardware configuration 							*/
	hwConfig.spiClkDiv	= SPI_CLK_DIV;
	hwConfig.wLen		= SPI_WORD_LENGTH_8;
	hwConfig.frLen		= SPI_FRAME_LENGTH;
	hwConfig.wcEnable	= SPI_WORD_IRQ_ENABLE;
	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
	hwConfig.csNum		= SPI_CS_NUM_1;
	hwConfig.dataDelay	= SPI_DATA_DLY_0;
	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

	result = SPI_config(hSpi, &hwConfig);
	if(CSL_SOK != result)
	{
		return (1);
	}
	else
	{
		LOG_printf (&trace, "SPI 1 Instance Configured successfully");
	}
	/* Reset the LCD */
	lcd_reset(hSpi);
	// Set LCD_A0 to 0
	//set_LCD_A0(0);

	/* Send Commands */

	lcd_cmd(hSpi, ST7565_LCD_CMD_SET_BIAS_9); // Setup 1/9th Bias Level: 1/33 duty 1/6 bias
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_ADC_NORMAL); // ADC Select
    //lcd_cmd(hSpi, ST7565_LCD_CMD_DISPLAY_OFF);
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_COM_NORMAL); // SHL Select
    // Set LCD operating voltage (regulator resistor, ref voltage resistor)
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_RESISTOR_RATIO | 0x0); //0x3);
    lcd_bright(hSpi, ST7565_LCD_PARAM_BRIGHTNESS | 0x3F);
    // Turn On voltage converter (VC=1, VR=0, VF=0)
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_POWER_CONTROL | 0x4);
    //PAL_osWaitMsecs(50);
    // Turn On voltage regulator (VC=1, VR=1, VF=0)
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_POWER_CONTROL | 0x6);
    //PAL_osWaitMsecs(50);
    // Turn on voltage follower (VC=1, VR=1, VF=1)
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_POWER_CONTROL | 0x7);
    //PAL_osWaitMsecs(100);
    lcd_cmd(hSpi, ST7565_LCD_CMD_DISPLAY_ON);
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_ALLPTS_NORMAL);
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_DISP_START_LINE | 32);  // Initial Display Line = 0

    /* Finally Clean up the LCD */
    return lcd_clear(hSpi);
}

/**
 * @brief Function to Put the LCD into Deep Sleep mode
 *
 * @param None
 */
void lcd_sleep ( CSL_SpiHandle hSpi )
{
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_STATIC_OFF);
    lcd_cmd(hSpi, ST7565_LCD_CMD_DISPLAY_OFF);
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_ALLPTS_ON);
}
/**
 * @brief Function to bring back the LCD from Deep Sleep mode
 *    All display contents are cleared and need to reinitalize
 *
 * @param None
 */
void lcd_wakeup ( CSL_SpiHandle hSpi )
{
    lcd_cmd(hSpi, ST7565_LCD_CMD_INTERNAL_RESET);
    lcd_bright(hSpi, ST7565_LCD_PARAM_BRIGHTNESS);
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_ALLPTS_NORMAL);
    lcd_cmd(hSpi, ST7565_LCD_CMD_DISPLAY_ON);
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_STATIC_ON);
    lcd_cmd(hSpi, ST7565_LCD_CMD_SET_STATIC_REG | 0x03);
}
/**
 * @brief Function to Put and wakup the LCD in Standby mode
 *      In this mode the display contents are not lost
 *      its only not visible
 * @param bWakeUp Used to decide that if we need to enter or exit standby
 *        0 - Enters into the Standby mode
 *        1 - Exits the Standby mode
 */
void lcd_standby (CSL_SpiHandle hSpi, Uint8 bWakeUp )
{
  if (bWakeUp == 0)
  { /* Enter standby mode */
      lcd_cmd(hSpi, ST7565_LCD_CMD_SET_STATIC_ON);
      lcd_cmd(hSpi, ST7565_LCD_CMD_SET_STATIC_REG | 0x03);
      lcd_cmd(hSpi, ST7565_LCD_CMD_DISPLAY_OFF);
      lcd_cmd(hSpi, ST7565_LCD_CMD_SET_ALLPTS_ON);
  }
  else
  { /* Wake from standby */
      lcd_cmd(hSpi, ST7565_LCD_CMD_SET_ALLPTS_NORMAL);
      lcd_cmd(hSpi, ST7565_LCD_CMD_DISPLAY_ON);
  }
}
static void _lcd_process_putc_newline (CSL_SpiHandle hSpi)
{
  /* Increment and Set the Location of Cursor */
  ++gw_row;
  if (gw_row >= ST7565_LCD_MAX_ROWS) /* Check if we are at the edge of the Screen */
  {
    gw_row = 0;
  }
  lcd_goto(hSpi, 0, gw_row);
}

void lcd_putc (CSL_SpiHandle hSpi, char c )
{
  Uint8 data, i;
  Uint8 *pFont;
  unsigned uoffset;

#if 0
  data = (Uint8) ((Uint8) c & 0x7FU); /* Filter out the Higher Range */
#endif
  data = ((Uint8) c);
  /* Filter our the Lower Range */
  if (data < 32)
  {
    if (data == '\n') /* process the New Line */
    {
      _lcd_process_putc_newline(hSpi);
    }
    else if (data == ST7565_LCD_FMT_CURSOR)
    {
      /* Put Cursor */
      for (i = ST7565_LCD_PARAM_FONT_CHARWIDTH; i > 0 ; i--)
      {
        lcd_data(hSpi, 0xFC);
      }
    }
  }
  /* Get the Cursor shift for the current character - Need to avoid broken character writes */
  gw_column += ST7565_LCD_PARAM_FONT_CHARWIDTH;

  /* Check if we have spilled over the boundary */
  if (gw_column >= ST7565_LCD_MAX_COLUMNS)
  {
    _lcd_process_putc_newline(hSpi);
    /* Still needed as the current character would be written that
       the new line first Place*/
    gw_column += ST7565_LCD_PARAM_FONT_CHARWIDTH;
  }

  /* Compute the Font Offset - We need 16 bit offset for actual address
   computation so using a pointer as a 16bit storage */
  uoffset = (unsigned) (((Uint16)data - ST7565_LCD_PARAM_FONT_CHAR_MINVAL) *
    ST7565_LCD_PARAM_FONT_WIDTH);

  /* Get the Font Pointer */
  pFont =  (Uint8 *) ((Uint8 *)gca_font + uoffset);

  /* Get the Font to Screen */
  for (i = ST7565_LCD_PARAM_FONT_WIDTH; i > 0 ; i--, pFont++)
  {
    data = *pFont;
    lcd_data(hSpi, data);
  }
  /* For the Additional Char width to have spacing */
  for (i = (ST7565_LCD_PARAM_FONT_CHARWIDTH - ST7565_LCD_PARAM_FONT_WIDTH);
    i > 0 ; i--)
  {
    lcd_data(hSpi, 0x00);
  }
}
/**
 * @brief Function to print a string on LCD
 *      In this mode the display contents are not lost
 *      its only not visible
 * @param bWakeUp Used to decide that if we need to enter or exit standby
 *        0 - Enters into the Standby mode
 *        1 - Exits the Standby mode
 */
Int16 lcd_printstring(CSL_SpiHandle hSpi, char *a, Uint16 length)
{
	Uint16 i, len;

	len = strlen(a);
	if ((length != 0)&&(length <= len))
		len = length;
	if (len > ST7565_LCD_MAX_COLUMNS/ST7565_LCD_PARAM_FONT_CHARWIDTH+1) len = ST7565_LCD_MAX_COLUMNS/ST7565_LCD_PARAM_FONT_CHARWIDTH+1;
	for(i = 0; i < len; i++)
		lcd_putc(hSpi, a[i]);
	if(length > len)
		for(i = 0; i < length-len; i++)
			lcd_putc(hSpi, ' ');
	return 0;
}
#if 0
lcd_initdisplaymenu(CSL_SpiHandle hSpi, char *s[])
{
	for(i = 0; i < 8; i++)
		lcd_printstring(hSpi, s[i]);
}
#endif

/**
 * @brief Function to print volume, rate, depth bar graphs of
 *  digital potentiometers
 *      It takes the Volume, Rate... level the label of the potentiometer
 *      the number of potentiometers and the steps of the bar (granularity)
 * @param
 *        Vol: a potentiometer value full scale = 1023. It is scaled to 0-100
 *        Label: a string that shows the use of the potentiometer (Volume, Rate, Depth, Octave...
 *        NumPots: the number of potentiometers shown in the LCD, up to four. It is actually set to 1 by default
 *        Step: the granularity of the bar graph. It can be used to set "Step" different values on the bar graph
 */
void lcd_VolGraph(CSL_SpiHandle hSpi, Uint16 *Vol, char* Label[], char NumPots, char *Step)
{
	char VolGraph[POTENTIOMETER];
	int i, j;

	for(i = 0; i < NumPots; i++)
	{
		VolGraph[i] = (Vol[i]*5)>>4;		// Convert Vol 0-1023 => 0-102
		if (VolGraph[i] > VOLGRAPH_SIZE-1) VolGraph[i] = VOLGRAPH_SIZE-1; // Convert Vol 0-1023 => 0-99
		lcd_goto(hSpi, 0, i);
		lcd_printstring(hSpi, Label[i], (ST7565_LCD_PARAM_WIDTH-VOLGRAPH_SIZE)/ST7565_LCD_PARAM_FONT_CHARWIDTH); //ST7565_LCD_MAX_COLUMNS/ST7565_LCD_PARAM_FONT_CHARWIDTH
		lcd_putc(hSpi, ' ');
		lcd_goto(hSpi, ST7565_LCD_PARAM_WIDTH-VOLGRAPH_SIZE-3, i);
		lcd_data(hSpi, 0xFF);	// Draw min line
		if (Step[i] != 0)
		{
			for(j = 1; j < VOLGRAPH_SIZE; j++)
			{
				lcd_goto(hSpi, ST7565_LCD_PARAM_WIDTH-VOLGRAPH_SIZE+j-3, i);
				lcd_data(hSpi, (((VolGraph[i]+Step[i]/2)*Step[i]/VOLGRAPH_SIZE*VOLGRAPH_SIZE/Step[i])>=j) ? 0x3C : 0x00); // draw a bar if Vol >= column
			}
			for(j = 1; j <= Step[i]; j++)
			{
				lcd_goto(hSpi, ST7565_LCD_PARAM_WIDTH-VOLGRAPH_SIZE+j*VOLGRAPH_SIZE/Step[i]-3, i);
				lcd_data(hSpi, 0xFF);	// Draw step lines
			}
		}
		else
		{
			for(j = 1; j < VOLGRAPH_SIZE; j++)
			{
				lcd_goto(hSpi, ST7565_LCD_PARAM_WIDTH-VOLGRAPH_SIZE+j-3, i);
				lcd_data(hSpi, (VolGraph[i]>=j) ? 0x3C : 0x00); // draw a bar if Vol >= column
			}
			lcd_goto(hSpi, ST7565_LCD_PARAM_WIDTH-3, i);
			lcd_data(hSpi, 0xFF);	// Draw max line
		}
	}
}

void lcd_DrawTunerCursors(CSL_SpiHandle hSpi, Uint8 row)
{
	Uint8 i;
	Int8 bar;

	// It draws an 8-pixels vertical line on column 8
	lcd_goto(hSpi, 8, row);
	lcd_data(hSpi, 0xFF);
	// It draws a vertical line on column 36 with different length on each row
	lcd_goto(hSpi, 36, row);
	if (row == 0)
		lcd_data(hSpi, 0xF0);
	else if (row == 3)
		lcd_data(hSpi, 0xFF);
	else
		lcd_data(hSpi, 0x0F);

	// It draws a 16-pixels base triangle cursor around column 64
	lcd_goto(hSpi, 56, row);
	if (row == 2)
		bar = 0x1;
	else
		bar = 0x80;
	lcd_data(hSpi, bar);
	for(i = 0; i < 7; i++)
	{
		if (row == 2)
		{
			bar <<= 1;
			bar |= 0x1;
		}
		else
		{
			bar >>= 1;
			bar |= 0x80; 	// sign extension
		}
		lcd_data(hSpi, bar);
	}
	for(i = 0; i < 7; i++)
	{
		if (row == 2)
		{
			bar >>= 1;
		}
		else
		{
			bar <<= 1;
		}
		lcd_data(hSpi, bar);
	}
	// It draws a vertical line on column 92 with different length on each row
	lcd_goto(hSpi, 92, row);
	if (row == 0)
		lcd_data(hSpi, 0xF0);
	else if (row == 3)
		lcd_data(hSpi, 0xFF);
	else
		lcd_data(hSpi, 0x0F);
	// It draws an 8-pixels vertical line on column 120
	lcd_goto(hSpi, 120, row);
	lcd_data(hSpi, 0xFF);
}

void lcd_Tuner(CSL_SpiHandle hSpi)
{
	int idx;
	char *pNotes;

	// draw a scale with a cursor in the middle on row 0
	lcd_DrawTunerCursors(hSpi, 0);
	// draw tuned notes on row 1
	idx = Tuner(GetPeakPeriod());
	pNotes = &Notes[0][0] + idx/8;
	lcd_goto(hSpi, idx%8, 1);
	lcd_printstring(hSpi, pNotes, 16);
	// draw a scale with a cursor in the middle on row 2
	lcd_DrawTunerCursors(hSpi, 2);
#if 0
	lcd_DrawTunerCursors(hSpi, 3);
#endif
}

lcd_printdisplay(CSL_SpiHandle hSpi)
{
	ContextMenu_t *pCtx;
	Uint16 i;
	static char *Label[POTENTIOMETER];
	static char Step[POTENTIOMETER];

	if((LcdCtx == &MenuTuner[0]))
	{
		lcd_Tuner(hSpi);
	}
	else if(LcdCtx == &MenuPot[0])
	{
#if (POTENTIOMETER<=2)
		SAR_SetCh_StrtCnv(SarHandle, 2);	// Starts next SAR single conversion and sets new channel
#else
    	SAR_SetCh_StrtCnv(SarHandle, chanNo);	// Starts next SAR single conversion and sets new channel
		i = 0;
		pCtx = LcdCtx->LeftContext;
		do
		{
			MenuPot[i].LeftContext = pCtx;
			Label[i] = &(pCtx->MenuStr[3]);				//skips "#  " in the string
			if (pCtx == &MenuPhShift[0])
			{
				Step[i] = 12;
				FxCmd[pCtx->Cmd] =  (SarData[i]+6)/20;
			}
			else if ((pCtx == &MenuEqParam[0])||(pCtx == &MenuEqParam[1])||(pCtx == &MenuEqParam[2])||(pCtx == &MenuEqParam[3]))
			{
				Step[0] = 11;
				FxCmd[pCtx->Cmd] =  SarData[2]/25;
			}
			else
			{
				Step[i] = 0;
				FxCmd[pCtx->Cmd] = SarData[i];
			}
			i++;
			pCtx = pCtx->DownContext;
		}
		while ((pCtx != &MenuMainSet[2])&&(pCtx->RightContext == &MenuPot[0])&&(i<POTENTIOMETER));
#endif
		pCtx = LcdCtx->LeftContext;
		MenuPot[0].LeftContext = pCtx;
		Label[0] = &(pCtx->MenuStr[3]);				//skips "#  " in the string
		if (pCtx == &MenuPhShift[0])
		{
			Step[0] = 6;
			FxCmd[pCtx->Cmd] =  SarData[2]/42;
		}
		else if (pCtx == &MenuFlanger[5])
		{
			Step[0] = 12;
			FxCmd[pCtx->Cmd] =  SarData[2]/21;
		}
		else if ((pCtx == &MenuEqParam[0])||(pCtx == &MenuEqParam[1])||(pCtx == &MenuEqParam[2])||(pCtx == &MenuEqParam[3]))
		{
			Step[0] = 11;
			FxCmd[pCtx->Cmd] =  SarData[2]/25;
		}
		else
		{
			Step[0] = 0;
			FxCmd[pCtx->Cmd] = SarData[2];
		}
		pCtx = pCtx->DownContext;
		lcd_VolGraph(hSpi, &SarData[2], Label, 1, Step);
	}
	else
	{
		lcd_goto(hSpi, 0, 0);
		if(((LcdCtx->RightContext==&MenuPot[0])&&(FxCmd[LcdCtx->Cmd] != 0))||
		   ((LcdCtx->RightContext!=&MenuPot[0])&&(FxCmd[LcdCtx->Cmd] == LcdCtx->Val)))
		{
			LcdCtx->MenuStr[2] = 254;
		}
		else
			LcdCtx->MenuStr[2] = ' ';
		lcd_printstring(hSpi,  LcdCtx->MenuStr, ST7565_LCD_MAX_COLUMNS/ST7565_LCD_PARAM_FONT_CHARWIDTH);
		pCtx = LcdCtx;
		for(i = 1; i <= 3; i++)
		{
			pCtx = pCtx->DownContext;
			lcd_goto(hSpi, 0, i);
			if(((pCtx->RightContext==&MenuPot[0])&&(FxCmd[pCtx->Cmd] != 0))||
			   ((pCtx->RightContext!=&MenuPot[0])&&(FxCmd[pCtx->Cmd] == pCtx->Val)))
			{
				pCtx->MenuStr[2] = 254;
			}
			else
				pCtx->MenuStr[2] = ' ';
			if(pCtx != LcdCtx)
				lcd_printstring(hSpi,  pCtx->MenuStr, ST7565_LCD_MAX_COLUMNS/ST7565_LCD_PARAM_FONT_CHARWIDTH);
			else
			{
				lcd_printstring(hSpi,  "", ST7565_LCD_MAX_COLUMNS/ST7565_LCD_PARAM_FONT_CHARWIDTH);
				pCtx = pCtx->UpContext;
			}
		}
	}
	switch (NavigatorButton)
	{
	case(DOWN_BUTTON):
		if((LcdCtx != &MenuPot[0])&&(LcdCtx != &MenuTuner[0]))
			LcdCtx = LcdCtx->DownContext;
		NavigatorButton = 0;
		break;
	case(UP_BUTTON):
		if((LcdCtx != &MenuPot[0])&&(LcdCtx != &MenuTuner[0]))
			LcdCtx = LcdCtx->UpContext;
		NavigatorButton = 0;
		break;
	case(LEFT_BUTTON):
		if(LcdCtx == &MenuTuner[0])
			sar_tune_deinit();
		else if (LcdCtx == &MenuNoise[0])
			NoiseSup_on = 0;
		if (LcdCtx->LeftContext != NULL)
			LcdCtx = LcdCtx->LeftContext;
		NavigatorButton = 0;
		break;
	case(RIGHT_BUTTON):
		if(LcdCtx->RightContext == &MenuPot[0])
		{
	#if (POTENTIOMETER<=2)
			SAR_SetCh_StrtCnv(SarHandle, 2);	// Starts next SAR single conversion and sets new channel
	#else
	    		SAR_SetCh_StrtCnv(SarHandle, chanNo);	// Starts next SAR single conversion and sets new channel
			i = 0;
			pCtx = LcdCtx;
			do
			{
				MenuPot[i].LeftContext = pCtx;
				Label[i] = &(pCtx->MenuStr[3]);				//skips "#  " in the string
				if (pCtx == &MenuPhShift[0])
				{
					Step[i] = 12;
					FxCmd[pCtx->Cmd] =  (SarData[i]+6)*12/1020;
				}
				else if ((pCtx == &MenuEqParam[0])||(pCtx == &MenuEqParam[1])||(pCtx == &MenuEqParam[2])||(pCtx == &MenuEqParam[3]))
				{
					Step[0] = 11;
					FxCmd[pCtx->Cmd] =  SarData[2]/25;
				}
				else
				{
					Step[i] = 0;
					FxCmd[pCtx->Cmd] = SarData[i];
				}
				i++;
				pCtx = pCtx->DownContext;
			}
			while ((pCtx != &MenuMainSet[2])&&(pCtx->RightContext == &MenuPot[0])&&(i<POTENTIOMETER));
	#endif
			pCtx = LcdCtx;
			MenuPot[0].LeftContext = pCtx;
			Label[0] = &(pCtx->MenuStr[3]);				//skips "#  " in the string
			if (pCtx == &MenuPhShift[0])
			{
				Step[0] = 6;
				FxCmd[pCtx->Cmd] =  SarData[2]/42;
			}
			else if (pCtx == &MenuFlanger[5])
			{
				Step[0] = 12;
				FxCmd[pCtx->Cmd] =  SarData[2]/21;
			}
			else if ((pCtx == &MenuEqParam[0])||(pCtx == &MenuEqParam[1])||(pCtx == &MenuEqParam[2])||(pCtx == &MenuEqParam[3]))
			{
				Step[0] = 11;
				FxCmd[pCtx->Cmd] =  SarData[2]/25;
			}
			else
			{
				Step[0] = 0;
				FxCmd[pCtx->Cmd] = SarData[2];
			}
			pCtx = pCtx->DownContext;
			lcd_VolGraph(hSpi, &SarData[2], Label, 1, Step);
		}
		else if (LcdCtx->RightContext == &MenuTuner[0])
		{
			lcd_clear(hSpi);
		    sar_tune_init();
		}
		else if(LcdCtx == &MenuTuner[0])
			sar_tune_deinit();
		else if (LcdCtx->RightContext == &MenuNoise[0])
		{
			NoiseSup_on = 1;
			SAR_SetCh_StrtCnv(SarHandle, 1);	// start the next conversion of Noise Suppression channel 1 (GPAIN1 - CH3)
		}
		else if (LcdCtx == &MenuNoise[0])
			NoiseSup_on = 0;
		FxCmd[LcdCtx->Cmd] = LcdCtx->Val;
		if(LcdCtx->RightContext != NULL)
		{
			LcdCtx = LcdCtx->RightContext;
		}
		NavigatorButton = 0;
		break;
	case(CENTER_BUTTON):
#if 0
		if(LcdCtx->RightContext == &MenuPot[0])
		{
	#if (POTENTIOMETER<=2)
			SAR_SetCh_StrtCnv(SarHandle, 2);	// Starts next SAR single conversion and sets new channel
	#else
	    	SAR_SetCh_StrtCnv(SarHandle, chanNo);	// Starts next SAR single conversion and sets new channel
			i = 0;
			pCtx = LcdCtx;
			do
			{
				MenuPot[i].LeftContext = pCtx;
				Label[i] = &(pCtx->MenuStr[3]);				//skips "#  " in the string
				if (pCtx == &MenuPhShift[0])
				{
					Step[i] = 12;
					FxCmd[pCtx->Cmd] =  (SarData[i]+6)*12/1020;
				}
				else if ((pCtx == &MenuEqParam[0])||(pCtx == &MenuEqParam[1])||(pCtx == &MenuEqParam[2])||(pCtx == &MenuEqParam[3]))
				{
					Step[0] = 11;
					FxCmd[pCtx->Cmd] =  SarData[2]/25;
				}
				else
				{
					Step[i] = 0;
					FxCmd[pCtx->Cmd] = SarData[i];
				}
				i++;
				pCtx = pCtx->DownContext;
			}
			while ((pCtx != &MenuMainSet[2])&&(pCtx->RightContext == &MenuPot[0])&&(i<POTENTIOMETER));
	#endif
			pCtx = LcdCtx;
			MenuPot[0].LeftContext = pCtx;
			Label[0] = &(pCtx->MenuStr[3]);				//skips "#  " in the string
			if (pCtx == &MenuPhShift[0])
			{
				Step[0] = 12;
				FxCmd[pCtx->Cmd] =  SarData[2]/22;
			}
			else if ((pCtx == &MenuEqParam[0])||(pCtx == &MenuEqParam[1])||(pCtx == &MenuEqParam[2])||(pCtx == &MenuEqParam[3]))
			{
				Step[0] = 11;
				FxCmd[pCtx->Cmd] =  SarData[2]/25;
			}
			else
			{
				Step[0] = 0;
				FxCmd[pCtx->Cmd] = SarData[2];
			}
			pCtx = pCtx->DownContext;
			lcd_VolGraph(hSpi, &SarData[2], Label, 1, Step);
		}
		else
#endif
		if (LcdCtx->RightContext == &MenuTuner[0])
		{
			lcd_clear(hSpi);
		    sar_tune_init();
		}
		else if(LcdCtx == &MenuTuner[0])
			sar_tune_deinit();
		else if (LcdCtx->RightContext == &MenuNoise[0])
		{
			NoiseSup_on = 1;
			SAR_SetCh_StrtCnv(SarHandle, 1);	// start the next conversion of Noise Suppression channel 1 (GPAIN1 - CH3)
		}
		else if (LcdCtx == &MenuNoise[0])
			NoiseSup_on = 0;
		if(FxCmd[LcdCtx->Cmd] == 0)
			FxCmd[LcdCtx->Cmd] = 1;
		else if(FxCmd[LcdCtx->Cmd] == LcdCtx->Val)
			FxCmd[LcdCtx->Cmd] = 0;
		else
			FxCmd[LcdCtx->Cmd] = LcdCtx->Val;
#if 0
		if(LcdCtx->RightContext == NULL)
			LcdCtx = LcdCtx->LeftContext;
		else
			LcdCtx = LcdCtx->RightContext;
#endif
		NavigatorButton = 0;
		break;
	}
}

void LcdMenu(void)
{
  	CSL_SpiHandle hSpi = NULL;
#if DEBUG>=10
  	Uint16 semcnt = 0;
#endif
  	Uint16 lcdstat = 1;
  	Uint16 i = 0;

  	for(i = 0; (lcdstat!=0)&&(i < 2); i++)
  		lcdstat = lcd_init(hSpi);  		// run twice if it didn't initialize properly
	while(1)
	{
		lcd_printdisplay(hSpi);
#ifdef STS_ENABLE
		STS_add(&STS_LcdMenu, 0); // debug
#endif
#if DEBUG>=10
    	semcnt = SEM_count(&SEM_Menu);
    	LOG_printf(&trace, "SEM_Menu post count = %d", semcnt);
#endif
    	SEM_post(&SEM_Menu);
#if 0
    	if(chanNo == 0)
	#if (POTENTIOMETER<=2)
			SAR_SetCh_StrtCnv(SarHandle, chanNo+2);	// Starts next SAR single conversion and sets channel
	#else
			SAR_SetCh_StrtCnv(SarHandle, chanNo);	// Starts next SAR single conversion and sets channel
	#endif
#endif
		TSK_sleep(300);
	}
}
#if 0
/************************************************************************/
/* Actual Execution                                                     */
/************************************************************************/

/**
 * @brief Main Function
 *
 * @param argc Number of Arguments
 * @param argv Passed Argument values in an Array
 *
 * @return Program final Return status
 */
CSL_Status lcd_test(int argv)
{
    Int16 result = 0;
  	CSL_SpiHandle hSpi = NULL;
    int i, j;
  	int argv2 = 1, argv3 = 1;
  	char *argv4[17] = {"Rezzonics - 2015", "  DMFX-1 v1.0   ", "Digital Multi-FX", " Coming soon!!! "};
#if 0
  	Uint16 Vol[] = {0, 240
#if (POTENTIOMETER==4)
  			, 480, 720};
#endif

  	};
#endif
  	char *Label[POTENTIOMETER] = {"volume"
#if (POTENTIOMETER>=2)
  	, "tone"
#if (POTENTIOMETER>=4)
  	, "rate", "depth"
#endif
#endif
  	};
  	char Step[POTENTIOMETER] = {0
#if (POTENTIOMETER>=2)
  	, 0
#endif
#if (POTENTIOMETER>=4)
  	, 0, 0
#endif
  	};

  	/* Initialize the LCD */
  	//result = lcd_init(hSpi);
  	/* Enter Processing Loop */
    /* Based on Input Codes perform the Function */
    /* In case of bare minimum input or 'init' command */
  	switch(argv)
	{
  	case 1:
		{
		   /* Enter into Standby */
		  lcd_standby(hSpi, 0);
		}
		break;
  	case 2:
		{
		  /* Exit into Standby */
		  lcd_standby(hSpi, 1);
		  break;
		}
  	case 3:
		{
		  /* Clear the LCD */
		  result = lcd_clear(hSpi);
		  /* Wakeup LCD */
		  //lcd_wakeup(hSpi);
		  break;
		}
  	case 4:
		{
		  Uint8 row,col;
		  col = (Uint8) (argv2 & ST7565_LCD_MASK_COLUMNS);
		  row = (Uint8) (argv3 & ST7565_LCD_MASK_ROWS);
		  /* Go the Specific LCD Location */
		  result = lcd_goto(hSpi, col, row);
		  break;
		}
  	case 5:
		{
		  int i;
		  // Test pattern
		  for(i=0;i<64;i++)
		  {
			lcd_data(hSpi, 0x55);
			lcd_data(hSpi, 0xAA);
			lcd_data(hSpi, 0x55);
			lcd_data(hSpi, 0xAA);
			lcd_data(hSpi, 0x55);
			lcd_data(hSpi, 0xAA);
		  }
		  break;
		}
  	case 6:
		{
		  /* Print text in the Current Location */
		  for(j = 0; j < 8; j++)
		  {
  			  lcd_goto(hSpi, 0, j);
  			  lcd_printstring(hSpi,  argv4[j%4], 0);
  			  /*
			  for(i=0;argv4[j%4][i]!=0;i++) // Null Terminated String
			  {
				lcd_putc(hSpi, argv4[j%4][i]);
			  }
			  */
		  }
		  i = 32;
		  while(1){
			  lcd_cmd(hSpi, ST7565_LCD_CMD_SET_DISP_START_LINE | i);
			  (i == 0) ? i = 31 : i--;
			  PAL_osWaitMsecs(125);
		  }
		  break;
		}
  	case 7:
		{
		for(j = 0; j < 7; j++)
		{
		  lcd_goto(hSpi, 0, 7-j);
		  for(i=0; i<128 ;i++) // Null Terminated String
		  {
			lcd_data(hSpi, Rezzonics[j*128+i]);
		  }
		}
		i = 0;
		while(1){
		  lcd_cmd(hSpi, ST7565_LCD_CMD_SET_DISP_START_LINE | i);
		  (i == 0) ? i = 31 : i--;
		  PAL_osWaitMsecs(125);
		}
		break;
		}
  	case 8:
  		{
		while(1)
		{
			lcd_VolGraph(hSpi, SarData, Label, POTENTIOMETER, Step);
			PAL_osWaitMsecs(250);
		}
  		break;
  		}
  	case 9:
		while(1)
		{
			lcd_printdisplay(hSpi);
			PAL_osWaitMsecs(250);
		}
		break;
	}
  /* Check if the SPI handle is open */
	result = SPI_deInit();

	if(CSL_SOK != result)
	{
		return (1);
	}
	else
	{
		LOG_printf (&trace, "SPI Instance deIntialize successfully");
	}

	result = SPI_close(hSpi);

	if(CSL_SOK != result)
	{
		return (1);
	}
	else
	{
		LOG_printf (&trace, "SPI Instance Close successfully");
	}
	return (result);
}
#endif

