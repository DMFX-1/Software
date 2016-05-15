################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
csl_i2c_intc_example.obj: ../csl_i2c_intc_example.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5505 -g --define="_DEBUG" --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/c55/DMFX_1_1/c55xx_csl/ccs_v5.0_examples/i2c/CSL_I2C_IntcExample/Debug" --diag_warning=225 --large_memory_model --ptrdiff_size=16 --algebraic --asm_source=algebraic --preproc_with_compile --preproc_dependency="csl_i2c_intc_example.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


