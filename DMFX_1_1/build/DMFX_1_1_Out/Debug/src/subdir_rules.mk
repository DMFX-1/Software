################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/app_audio_alg.obj: C:/c55/DMFX_1_1/src/app_audio_alg.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/app_audio_alg.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/clean.obj: C:/c55/DMFX_1_1/src/clean.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/clean.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/codec_aic3254.obj: C:/c55/DMFX_1_1/src/codec_aic3254.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/codec_aic3254.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/compressor.obj: C:/c55/DMFX_1_1/src/compressor.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/compressor.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/csl_i2c_intc.obj: C:/c55/DMFX_1_1/src/csl_i2c_intc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/csl_i2c_intc.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/csl_sar_intc.obj: C:/c55/DMFX_1_1/src/csl_sar_intc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/csl_sar_intc.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/equalizer.obj: C:/c55/DMFX_1_1/src/equalizer.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/equalizer.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/gpio_control.obj: C:/c55/DMFX_1_1/src/gpio_control.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/gpio_control.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/i2s_sample.obj: C:/c55/DMFX_1_1/src/i2s_sample.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/i2s_sample.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.obj: C:/c55/DMFX_1_1/src/main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/main.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/phaser.obj: C:/c55/DMFX_1_1/src/phaser.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/phaser.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/pll_control.obj: C:/c55/DMFX_1_1/src/pll_control.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/pll_control.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/variabledelay.obj: C:/c55/DMFX_1_1/src/variabledelay.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"C:/ti/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" -v5535 --memory_model=large -O3 -g --include_path="C:/ti/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="C:/c55/DMFX_1_1/inc" --include_path="C:/c55/DMFX_1_1/build/DMFX_1_1_Out/Debug" --include_path="C:/c55/DMFX_1_1/c55xx_csl/inc" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c5500" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/dma/inc" --include_path="C:/c55/DMFX_1_1/dsplib/inc" --include_path="C:/c55/DMFX_1_1/asrc/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/inc" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2c/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/i2s/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/inc/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_os/src/bios/src/" --include_path="C:/c55/DMFX_1_1/c55x5_drivers/pal_sys/da225/src/" --gcc --define="_DEBUG" --define=BIOS_BUILD --define=CSL_AC_TEST --define=C5535_EZDSP_DEMO --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=16 --algebraic --preproc_with_compile --preproc_dependency="src/variabledelay.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


