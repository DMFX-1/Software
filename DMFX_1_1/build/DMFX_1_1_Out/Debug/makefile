################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/c5500_4.4.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./src/variabledelay.obj" \
"./src/pll_control.obj" \
"./src/phaser.obj" \
"./src/main.obj" \
"./src/i2s_sample.obj" \
"./src/gpio_control.obj" \
"./src/equalizer.obj" \
"./src/csl_sar_intc.obj" \
"./src/csl_i2c_intc.obj" \
"./src/compressor.obj" \
"./src/codec_aic3254.obj" \
"./src/clean.obj" \
"./src/app_audio_alg.obj" \
"./DMFX1_CSL_BIOS_cfg_c.obj" \
"./DMFX1_CSL_BIOS_cfg.obj" \
-l"Debug/dma_bios_drv_lib.lib" \
-l"Debug/i2c_bios_drv_lib.lib" \
-l"Debug/i2s_bios_drv_lib.lib" \
-l"Debug/palos_bios_lib.lib" \
-l"Debug/pal_sys_bios_lib.lib" \
-l"Debug/C55XXCSL_LP.lib" \
-l"asrc_dly_fix_lib.lib" \
-l"libc.a" \
-l"C:/c55/DMFX_1_1\dsplib\lib\55xdspl.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: DMFX.1.1.out

dependents:
	-cd C:/c55/DMFX_1_1/c55x5_drivers/dma/build/dma_bios_drv_lib/Debug && $(MAKE) all
	-cd C:/c55/DMFX_1_1/c55x5_drivers/i2c/build/i2c_bios_drv_lib/Debug && $(MAKE) all
	-cd C:/c55/DMFX_1_1/c55x5_drivers/i2s/build/i2s_bios_drv_lib/Debug && $(MAKE) all
	-cd C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/build/pal_sys_bios_lib/Debug && $(MAKE) all
	-cd C:/c55/DMFX_1_1/c55x5_drivers/pal_os/build/palos_bios_lib/Debug && $(MAKE) all
	-cd C:/c55/DMFX_1_1/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP/Debug && $(MAKE) all

# Tool invocations
DMFX.1.1.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS) C:/c55/DMFX_1_1/c55x5_drivers/dma/build/dma_bios_drv_lib/../../../../c55x5_drivers/lib/Debug/dma_bios_drv_lib.lib C:/c55/DMFX_1_1/c55x5_drivers/i2c/build/i2c_bios_drv_lib/../../../../c55x5_drivers/lib/Debug/i2c_bios_drv_lib.lib C:/c55/DMFX_1_1/c55x5_drivers/i2s/build/i2s_bios_drv_lib/../../../../c55x5_drivers/lib/Debug/i2s_bios_drv_lib.lib C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/build/pal_sys_bios_lib/../../../../c55x5_drivers/pal_sys/lib/debug/pal_sys_bios_lib.lib C:/c55/DMFX_1_1/c55x5_drivers/pal_os/build/palos_bios_lib/../../../../c55x5_drivers/pal_os/lib/debug/palos_bios_lib.lib C:/c55/DMFX_1_1/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP/Debug/C55XXCSL_LP.lib
	@echo 'Building target: $@'
	@echo 'Invoking: C5500 Linker'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic -z -m"DMFX.1.1.map" --stack_size=0x200 --heap_size=0x400 -i"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/lib" -i"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" -i"C:/ti/bios_5_42_01_09/packages/ti/rtdx/lib/c5500" -i"C:/ti/bios_5_42_01_09/packages/ti/bios/lib" -i"C:/c55/DMFX_1_1/build/DMFX_1_1_Out" -i"C:/c55/DMFX_1_1/build" -i"/packages/ti/rtdx/lib/c5000" -i"C:/c55/DMFX_1_1/asrc/lib" -i"C:/c55/DMFX_1_1/c55x5_drivers/lib/" -i"C:/c55/DMFX_1_1/c55x5_drivers/pal_os/lib/" -i"C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/lib/" -i"C:/c55/DMFX_1_1/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP/" --reread_libs --display_error_number --warn_sections --xml_link_info="DMFX.1.1_linkInfo.xml" --rom_model --sys_stacksize=0x200 -o "DMFX.1.1.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C5500_EXECUTABLE_OUTPUTS__QUOTED)$(GEN_HDRS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(GEN_CMDS__QUOTED) "DMFX.1.1.out"
	-$(RM) "DMFX1_CSL_BIOS_cfg_c.pp" "src\app_audio_alg.pp" "src\clean.pp" "src\codec_aic3254.pp" "src\compressor.pp" "src\csl_i2c_intc.pp" "src\csl_sar_intc.pp" "src\equalizer.pp" "src\gpio_control.pp" "src\i2s_sample.pp" "src\main.pp" "src\phaser.pp" "src\pll_control.pp" "src\variabledelay.pp" 
	-$(RM) "DMFX1_CSL_BIOS_cfg.pp" 
	-$(RM) "DMFX1_CSL_BIOS_cfg.obj" "DMFX1_CSL_BIOS_cfg_c.obj" "src\app_audio_alg.obj" "src\clean.obj" "src\codec_aic3254.obj" "src\compressor.obj" "src\csl_i2c_intc.obj" "src\csl_sar_intc.obj" "src\equalizer.obj" "src\gpio_control.obj" "src\i2s_sample.obj" "src\main.obj" "src\phaser.obj" "src\pll_control.obj" "src\variabledelay.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
C:/c55/DMFX_1_1/c55x5_drivers/dma/build/dma_bios_drv_lib/../../../../c55x5_drivers/lib/Debug/dma_bios_drv_lib.lib:
C:/c55/DMFX_1_1/c55x5_drivers/i2c/build/i2c_bios_drv_lib/../../../../c55x5_drivers/lib/Debug/i2c_bios_drv_lib.lib:
C:/c55/DMFX_1_1/c55x5_drivers/i2s/build/i2s_bios_drv_lib/../../../../c55x5_drivers/lib/Debug/i2s_bios_drv_lib.lib:
C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/build/pal_sys_bios_lib/../../../../c55x5_drivers/pal_sys/lib/debug/pal_sys_bios_lib.lib:
C:/c55/DMFX_1_1/c55x5_drivers/pal_os/build/palos_bios_lib/../../../../c55x5_drivers/pal_os/lib/debug/palos_bios_lib.lib:
C:/c55/DMFX_1_1/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP/Debug/C55XXCSL_LP.lib:

-include ../makefile.targets

