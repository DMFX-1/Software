################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/csl_i2c_intc.obj: ../src/csl_i2c_intc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/csl_i2c_intc.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/csl_sar_intc.obj: ../src/csl_sar_intc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/csl_sar_intc.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/csl_spi_flash.obj: ../src/csl_spi_flash.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/csl_spi_flash.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/gpio_control.obj: ../src/gpio_control.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/gpio_control.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/gpio_intc.obj: ../src/gpio_intc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/gpio_intc.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/lcdST7565.obj: ../src/lcdST7565.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/lcdST7565.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.obj: ../src/main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/main.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/pll_control.obj: ../src/pll_control.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/pll_control.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/tuner.obj: ../src/tuner.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX.1.2/inc" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX.1.2/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1./c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --define=c5535 --define=C5535_EZDSP_DEMO --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/tuner.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


