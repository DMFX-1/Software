/********************************************************************
* file: fxcmd.h
*
* Brief: This file contains FxCmd array definition
*
*********************************************************************/
#ifndef _FX_CMD_H_
#define _FX_CMD_H_

// FxCmd SAR ADC pot values go from 0 to 1023
Uint16 FxCmd[FXCMD_SIZE] = {
		0,  			//0x0 :	Reserved
		0,  			//0x1 :	Main Settings
			0,  		//0x2 :		Bypass
			0,  		//0x3 :		Headphones volume
			0,			//0x4 :		Line-Out volume
			24,			//0x5 :		Analog Gain
			0,  		//0x6 :		Tuner
			0,  		//0x7 :		Sampling Frequency
		0,  			//0x8 :	Patch
			1,  		//0x9 :		Patch 1
			2,  		//0xA :		Patch 2
			3,  		//0xB :		Patch 3
			0,  		//0xC :		Patch 4
			0,  		//0xD :		Patch 5
			0,  		//0xE :		Patch 6
			0,  		//0xF :		Patch 7
			0,  		//0x10:		Patch 8
		0,  			//0x11:	Record
			0,  		//0x12: 	Select
		0,  			//0x13: Compressor
			255,  		//0x14: 	Sustain
			10,  		//0x15:  	Attack
			250,  		//0x16:  	Delay
			255,  		//0x17:  	Level
		0,  			//0x18: Distortion
			1,  		//0x19: 	Type
				7,  	//0x1A: 		Vintage, Warm, Crunch, Blues, Hard, Metal, Fuzz, General
					1,  //0x1B:				Drive 0
					1,	//0x1C:				Drive 1
					1,	//0x1D:				Mode 0
					1,	//0x1E:				Mode 1
			255,  		//0x1F: 	Drive
			0,  		//0x20: 	Tone
			255,  		//0x21: 	Level
		0,  			//0x22: Equalizer
			9,  		//0x23: 	Level
			5,  		//0x24: 	Bass
			5,  		//0x25: 	Middle
			5,  		//0x26: 	Treble
		0,  			//0x27: Phaser
			255,  		//0x28: 	Rate
			255,  		//0x29: 	Depth
			255,  		//0x2A: 	Resonance
			1,  		//0x2B: 	Mono/Stereo
				3,  	//0x2C: 		Mono, Stereo, Leslie
		0,  			//0x2D: Noise Cancel
			100,  		//0x2E: 	Threshold
			0,  		//0x2F: 	Auto
		0,  			//0x30: Wah-Wah
			0,  		//0x31: 	Wah-Wah A
			0,  		//0x32: 	Wah-Wah B
			0,  		//0x33: 	Wah-Wah Reserved
		0,  			//0x34: Delay
			0,  		//0x35: 	Type
				0,  	//0x36: 		Normal, Pan, Echo, Tap
			94,  		//0x37: 	Delay
			25,  		//0x38: 	Rate
			255,  		//0x39: 	Depth
			255,  		//0x3A: 	FeedBack
			255,  		//0x3B: 	FeedFwd
			255,  		//0x3C: 	Blend
			1,  		//0x3D: 	LFO
				1,  	//0x3E: 		Sine, Noise, Triangle, Sawtooth
			1,  		//0x3F: 	Mono/Stereo
				3,  	//0x40: 		Mono, Stereo, Leslie
		0,  			//0x41: Echo
			0,  		//0x42: 	FeedBack
			255,  		//0x43: 	FeedFwd
			94,  		//0x44: 	Delay
			255,  		//0x45: 	Depth
			1,  		//0x46: 	Mono/Stereo
				3,  	//0x47: 		Mono, Stereo, Leslie
		0,  			//0x48: Chorus
			1,  		//0x49: 	Type
				2,  	//0x4A: 		Industry Std, White
			25,  		//0x4B: 	Rate
			255,  		//0x4C: 	Depth
			255,  		//0x4D: 	Delay
			1,  		//0x4E: 	Mono/Stereo
				3,  	//0x4F: 		Mono, Stereo, Leslie
		0,  			//0x50: Flanger
			128,  		//0x51: 	Rate
			255,  		//0x52: 	Depth
			255,  		//0x53: 	Delay
			255,  		//0x54: 	Resonance
			1,  		//0x55: 	Mono/Stereo
				3,  	//0x56: 		Mono, Stereo, Leslie
			255,  		//:0x57 	Notches
		0,  			//:0x58 Pitch Shift
			6,  		//:0x59 	Pitch
			128,  		//:0x5A 	Mix
			255,  		//:0x5B 	Effect level
		0,  			//:0x5C Tremolo
			128,  		//:0x5D 	Rate
			255,  		//:0x5E 	Depth
			255,  		//:0x5F	 	Delay
		0,  			//:0x60 Reverb
			255,  		//:0x61 	Delay (Room, Hall, Stadium, Cathedral)
			255, 	 	//:0x62 	Dry Level
			255,  		//:0x63 	Early Level
			255   		//:0x64 	Late level
};
#endif
