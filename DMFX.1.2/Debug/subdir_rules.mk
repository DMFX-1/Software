################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
DMFX2_CSL_BIOS_cfg.cmd: C:/c55/DMFX.1.2/Debug/DMFX2_CSL_BIOS_cfg.tcf
	@echo 'Building file: $<'
	@echo 'Invoking: TConf'
	"C:/ti/bios_5_42_01_09/xdctools/tconf" -b -Dconfig.importPath="C:/ti/bios_5_42_01_09/packages;" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DMFX2_CSL_BIOS_cfg.s??: | DMFX2_CSL_BIOS_cfg.cmd
DMFX2_CSL_BIOS_cfg_c.c: | DMFX2_CSL_BIOS_cfg.cmd
DMFX2_CSL_BIOS_cfg.h: | DMFX2_CSL_BIOS_cfg.cmd
DMFX2_CSL_BIOS_cfg.h??: | DMFX2_CSL_BIOS_cfg.cmd
DMFX2_CSL_BIOS_.cdb: | DMFX2_CSL_BIOS_cfg.cmd

DMFX2_CSL_BIOS_cfg.obj: ./DMFX2_CSL_BIOS_cfg.s?? $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="DMFX2_CSL_BIOS_cfg.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

DMFX2_CSL_BIOS_cfg_c.obj: ./DMFX2_CSL_BIOS_cfg_c.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="DMFX2_CSL_BIOS_cfg_c.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


