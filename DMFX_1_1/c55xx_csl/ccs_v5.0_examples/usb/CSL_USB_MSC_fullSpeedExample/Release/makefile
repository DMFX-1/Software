################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./csl_usb_msc_fullspeed_example.obj" \
"./chk_mmc.obj" \
"./VC5505_CSL_BIOS_cfg_c.obj" \
"./VC5505_CSL_BIOS_cfg.obj" \
-l"Release/C55XXCSL_LP.lib" \
-l"rts55x.lib" \
-l"C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/usb/CSL_USB_MSC_fullSpeedExample/../../../ccs_v5.0_examples/drv/lib/Debug/atafs_bios_drv_lib.lib" \
-l"rts55h.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
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
all: CSL_USB_MSC_fullSpeedExample_Out.out

dependents:
	-cd C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP/Release && $(MAKE) all
	-cd C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/atafs/atafs_bios_drv_lib/Release && $(MAKE) all

# Tool invocations
CSL_USB_MSC_fullSpeedExample_Out.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS) C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP/Release/C55XXCSL_LP.lib C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/atafs/atafs_bios_drv_lib/../../../../ccs_v5.0_examples/drv/lib/Release/atafs_bios_drv_lib.lib
	@echo 'Building target: $@'
	@echo 'Invoking: C5500 Linker'
	"C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 -g -O3 --define="CSL_MSC_TEST" --define=c5505 --diag_warning=225 --large_memory_model --ptrdiff_size=16 --no_nops_in_delay --algebraic --asm_source=algebraic -z -m"CSL_USB_MSC_fullSpeedExample_Out.map" --stack_size=0x200 --heap_size=0x400 --warn_sections -i"C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/lib" -i"C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/include" -i"C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/rtdx/lib/c5500" -i"C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/bios/lib" -i"C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP" -i"C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/usb/CSL_USB_MSC_fullSpeedExample" -i"/packages/ti/rtdx/lib/c5000" --reread_libs --xml_link_info="CSL_USB_MSC_fullSpeedExample_Out_linkInfo.xml" --rom_model --sys_stacksize=0x200 -o "CSL_USB_MSC_fullSpeedExample_Out.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C5500_EXECUTABLE_OUTPUTS__QUOTED)$(GEN_HDRS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(GEN_CMDS__QUOTED) "CSL_USB_MSC_fullSpeedExample_Out.out"
	-$(RM) "VC5505_CSL_BIOS_cfg_c.pp" "chk_mmc.pp" "csl_usb_msc_fullspeed_example.pp" 
	-$(RM) "VC5505_CSL_BIOS_cfg.pp" 
	-$(RM) "VC5505_CSL_BIOS_cfg.obj" "VC5505_CSL_BIOS_cfg_c.obj" "chk_mmc.obj" "csl_usb_msc_fullspeed_example.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/C55XXCSL_LP/Release/C55XXCSL_LP.lib:
C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/atafs/atafs_bios_drv_lib/../../../../ccs_v5.0_examples/drv/lib/Release/atafs_bios_drv_lib.lib:

-include ../makefile.targets

