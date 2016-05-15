################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
VC5505_CSL_BIOS_cfg.cmd: ../VC5505_CSL_BIOS_cfg.tcf
	@echo 'Building file: $<'
	@echo 'Invoking: TConf'
	"C:/ti/ccs5_5/bios_5_42_01_09/xdctools/tconf" -b -Dconfig.importPath="C:/ti/ccs5_5/bios_5_42_01_09/packages;" "$<"
	@echo 'Finished building: $<'
	@echo ' '

VC5505_CSL_BIOS_cfg.s??: | VC5505_CSL_BIOS_cfg.cmd
VC5505_CSL_BIOS_cfg_c.c: | VC5505_CSL_BIOS_cfg.cmd
VC5505_CSL_BIOS_cfg.h: | VC5505_CSL_BIOS_cfg.cmd
VC5505_CSL_BIOS_cfg.h??: | VC5505_CSL_BIOS_cfg.cmd
VC5505_CSL_BIOS_.cdb: | VC5505_CSL_BIOS_cfg.cmd

VC5505_CSL_BIOS_cfg.obj: ./VC5505_CSL_BIOS_cfg.s?? $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 -g -O3 --define="CSL_MSC_TEST" --define=c5505 --include_path="C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/inc" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/atafs/src" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/common" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/usb/CSL_USB_MSC_fullSpeedExample/Release" --diag_warning=225 --large_memory_model --ptrdiff_size=16 --no_nops_in_delay --algebraic --asm_source=algebraic --preproc_with_compile --preproc_dependency="VC5505_CSL_BIOS_cfg.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

VC5505_CSL_BIOS_cfg_c.obj: ./VC5505_CSL_BIOS_cfg_c.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 -g -O3 --define="CSL_MSC_TEST" --define=c5505 --include_path="C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/inc" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/atafs/src" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/common" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/usb/CSL_USB_MSC_fullSpeedExample/Release" --diag_warning=225 --large_memory_model --ptrdiff_size=16 --no_nops_in_delay --algebraic --asm_source=algebraic --preproc_with_compile --preproc_dependency="VC5505_CSL_BIOS_cfg_c.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

chk_mmc.obj: ../chk_mmc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 -g -O3 --define="CSL_MSC_TEST" --define=c5505 --include_path="C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/inc" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/atafs/src" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/common" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/usb/CSL_USB_MSC_fullSpeedExample/Release" --diag_warning=225 --large_memory_model --ptrdiff_size=16 --no_nops_in_delay --algebraic --asm_source=algebraic --preproc_with_compile --preproc_dependency="chk_mmc.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

csl_usb_msc_fullspeed_example.obj: ../csl_usb_msc_fullspeed_example.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 -g -O3 --define="CSL_MSC_TEST" --define=c5505 --include_path="C:/ti/ccs5_5/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/ccs5_5/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/inc" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/atafs/src" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/drv/common" --include_path="C:/Users/x0048282/Clone_CSL3_04/c55_csl/c55xx_csl/ccs_v5.0_examples/usb/CSL_USB_MSC_fullSpeedExample/Release" --diag_warning=225 --large_memory_model --ptrdiff_size=16 --no_nops_in_delay --algebraic --asm_source=algebraic --preproc_with_compile --preproc_dependency="csl_usb_msc_fullspeed_example.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


