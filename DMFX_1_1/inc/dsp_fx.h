/********************************************************************
* file: dsp_fx.h
*
* Brief: This file contains DSP FX types
*
*********************************************************************/
#ifndef _DSP_FX_H_
#define _DSP_FX_H_

// FX Command definitions
#define	    MAIN_SETTINGS		0x1
#define 	BYPASS				0x2
#define		HP_VOL				0x3
#define		LOUT_VOL			0x4
#define		ANA_GAIN			0x5
#define		TUNER				0x6
#define		SAMPLE_FREQ			0x7
#define		PATCH_MAIN			0x8
#define		PATCH_1				0x9
#define		PATCH_2				0xA
#define		PATCH_3				0xB
#define		PATCH_4				0xC
#define		PATCH_5				0xD
#define		PATCH_6				0xE
#define		PATCH_7				0xF
#define		PATCH_8				0x10
#define		PATCH_REC			0x11
#define		PATCH_SEL			0x12
#define		COMPRESSOR			0x13
#define		CMP_SUSTAIN			0x14
#define		CMP_ATTACK			0x15
#define		CMP_DECAY			0x16
#define		CMP_LEVEL			0x17
#define		DISTORTION			0x18
#define		DIST_TYPE			0x19
#define		DIST_TYPE_LIST		0x1A
#define		DRV1_8DB			0x1B
#define		DRV0_16DB			0x1C
#define		MD0_MIDS			0x1D
#define		MD1_CLIP			0x1E
#define		DIST_DRIVE			0x1F
#define		DIST_TONE			0x20
#define		DIST_LEVEL			0x21
#define		EQUALIZER			0x22
#define		EQ_LEVEL			0x23	// EQ_LEVEL, EQ_BASS, EQ_MIDDLE, EQ_TREBLE
#define		EQ_BASS				0x24	// must be consecutive and follow
#define		EQ_MIDDLE			0x25	// that order
#define		EQ_TREBLE			0x26
#define		PHASER				0x27
#define		PH_RATE				0x28
#define		PH_DEPTH			0x29
#define		PH_RESONANCE		0x2A
#define		PH_STEREO			0x2B
#define		PH_STEREO_LIST		0x2C
#define		NOISE_CANCEL		0x2D
#define		NS_THRESHOLD		0x2E
#define		NS_AUTO				0x2F
#define		WAH_WAH				0x30
#define		WAH_A				0x31
#define		WAH_B				0x32
#define		WAH_RESERVED		0x33
#define		DELAY				0x34
#define		DEL_TYPE			0x35
#define		DEL_TYPE_LIST		0x36
#define		DEL_DELAY			0x37
#define		DEL_RATE			0x38
#define		DEL_DEPTH			0x39
#define		DEL_FB				0x3A
#define		DEL_FF				0x3B
#define		DEL_BL				0x3C
#define		DEL_LFO			    0x3D
#define		DEL_LFO_LIST		0x3E
#define		DEL_STEREO			0x3F
#define		DEL_STEREO_LIST		0x40
#define		ECHO				0x41
#define		ECHO_FB             0x42
#define		ECHO_FF             0x43
#define		ECHO_DELAY          0x44
#define		ECHO_DEPTH          0x45
#define		ECHO_STEREO         0x46
#define		ECHO_STEREO_LIST    0x47
#define		CHORUS				0x48
#define		CH_TYPE				0x49
#define		CH_TYPE_LIST        0x4A
#define		CH_RATE				0x4B
#define		CH_DEPTH			0x4C
#define		CH_DELAY			0x4D
#define		CH_STEREO         	0x4E
#define		CH_STEREO_LIST    	0x4F
#define		FLANGER				0x50
#define		FL_RATE				0x51
#define		FL_DEPTH			0x52
#define		FL_DELAY			0x53
#define		FL_RESONANCE		0x54
#define		FL_STEREO           0x55
#define		FL_STEREO_LIST      0x56
#define		FL_NOTCHES			0x57
#define		PITCH_SHIFT			0x58
#define		PITCH_VAL			0x59
#define		PS_DLEVEL			0x5A
#define		PS_FXLEVEL			0x5B
#define		TREMOLO				0x5C
#define		TR_RATE				0x5D
#define		TR_DEPTH			0x5E
#define		TR_DELAY			0x5F
#define		REVERB				0x60
#define		REV_DELAY			0x61
#define		REV_DRY				0x62
#define		REV_EARLY			0x63
#define		REV_LATE			0x64
#define 	FXCMD_SIZE			0x65  // Last FX Command + 1

#define 	VINTAGE		1
#define 	WARM		2
#define 	CRUNCH 		3
#define 	BLUES  		4
#define 	HARD   		5
#define 	METAL  		6
#define 	FUZZ   		7

#if 0                        
typedef enum {
	EFFECT_OFF=0, 
	VOLUME=1, /*PATCH=2, COMPRESSOR=3,*/ 
	DISTORTION=2, 
	EQUALIZER=3, 
	PHASER=4 /*, NOISE_SUP=7, WAH=8, DELAY=9, CHORUS=10, FLANGER=11, PITCH_SHFT=12, TREMOLO=13, REVERB=14, MODULATION=15*/
} EffectType; 

typedef enum {
	DIST_TYPE_OFF=0, 
	BLUES=1, 
	WARM=2, 
	CRUNCH=3, 
	VINT=4, 
	HARD=5, 
	METAL=6, 
	FUZZ=7, 
	SHOEGAZE=8 
} DistType;

typedef enum {
	DIST_PARAM_OFF=0, 
	TYPE=1, 
	DRIVE=2, 
	TONE=3, 
	LEVEL=4
} DistParam; 

typedef enum {
	REVERB_TYPE_OFF=0, 
	ROOM=1, 
	HALL=2
} ReverbType;

typedef enum {
	EQ_TYPE_OFF=0, 
	TREBLE=1, 
	MIDDLE=2, 
	BASS=3
} EqParam;

typedef enum {
	PHASER_PARAM_OFF=0, 
	RATE=1, 
	DEPTH=2, 
	RESONANCE=3
} PhaserParam; 

typedef enum {
	INIT=0, 
	VOL0, VOL1, 
	DIST0, DISTTYPE0, DISTTYPE1, DISTDRIVE0, DISTDRIVE1, DISTTONE0, DISTTONE1, DISTLEVEL0, DISTLEVEL1, 
	EQ0, EQTREBLE0, EQTREBLE1, EQMIDDLE0, EQMIDDLE1, EQBASS0, EQBASS1, 
	PHASER0, PHRATE0, PHRATE1, PHDEPTH0, PHDEPTH1, 
	PHRES0, PHRES1
} MenuStatType;
/*typedef enum {VAL_OFF=-1, VAL_OFF, VAL_10, VAL_20, VAL_30, VAL_40, VAL_50, VAL_60, VAL_70, VAL_80, VAL_90, VAL_100} Val0100; 
typedef enum {-60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50} Val_50_50; 
typedef enum {-13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1,2,3,4,5,6,7,8,9,10,11,12} Pitch-12_12;*/
#endif
typedef enum {
    LCD_MSG_BUTTON_NONE = 0,
	/** left button pressed message                                          */
    LCD_MSG_BUTTON_L,
	/** right button pressed message                                         */
    LCD_MSG_BUTTON_R,
	/** Both button pressed message                                          */
    LCD_MSG_BUTTON_BOTH
} LcdMsgType;
#if 0
struct MenuTable
{
	EffectType Effect;
	unsigned short Volume;
	struct DistStruct{
		DistParam Param;
		DistType Type;
		unsigned short Drive;
		unsigned short Tone;
		unsigned short Level;
	} Distortion;
	struct EqStruct{
		EqParam Param;
		unsigned short Treble;
		unsigned short Middle;
		unsigned short Bass;
		unsigned short Level;
	} Equalizer;
	struct PhaserStruct{
		PhaserParam Param;
		unsigned short Rate;
		unsigned short Depth;
		unsigned short Resonance;
	} Phaser;
	ReverbType Reverb;
};

short menu(LcdMsgType LcdMsg);
short SetDistParams(void);
#endif
#endif
