################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/build/Debug/pal_sys.obj: C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/pal_sys.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -vcpu:3.3 --memory_model=large -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/build/pal_sys_bios_lib/Debug" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/ti/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc" --define="_DEBUG" --define="BIOS_BUILD" --define=c5535 --diag_warning=225 --ptrdiff_size=16 --obj_directory="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/build/Debug" --preproc_with_compile --preproc_dependency="pal_sys.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


