################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
c55x5_drivers/pal_sys/da225/src/pal_sys.obj: ../c55x5_drivers/pal_sys/da225/src/pal_sys.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/Debug" --include_path="C:/ti/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_00_07/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_00_07/packages/ti/rtdx/include/c5500" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/dma/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/dsplib/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/asrc/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/i2c/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/i2s/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/pal_os/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/pal_sys/inc" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/pal_os/src/bios/src" --include_path="C:/Documents and Settings/Diego de la Cruz/workspace_v5_3/DMFX-1-1/c55x5_drivers/pal_sys/da225/src" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --preproc_with_compile --preproc_dependency="c55x5_drivers/pal_sys/da225/src/pal_sys.pp" --obj_directory="c55x5_drivers/pal_sys/da225/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


