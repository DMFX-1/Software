################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
csl_i2c_polled_example.obj: ../csl_i2c_polled_example.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/c55xx_csl/ccs_v5.0_examples/i2c/CSL_I2C_PollExample/Debug" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc/" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --define="_DEBUG" --define=c5535 --diag_warning=225 --ptrdiff_size=16 --algebraic --asm_source=algebraic --preproc_with_compile --preproc_dependency="csl_i2c_polled_example.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


