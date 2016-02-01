################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/TexasInstruments/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/TexasInstruments/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/TexasInstruments/TivaWare_C_Series-2.1.2.111" --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RB1 --define=PART_TM4C123GH6PM --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c123gh6pm_startup_ccs.obj: ../tm4c123gh6pm_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/TexasInstruments/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/TexasInstruments/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/TexasInstruments/TivaWare_C_Series-2.1.2.111" --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RB1 --define=PART_TM4C123GH6PM --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="tm4c123gh6pm_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

uartstdio.obj: C:/TexasInstruments/TivaWare_C_Series-2.1.2.111/utils/uartstdio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/TexasInstruments/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/TexasInstruments/ccsv6/tools/compiler/ti-cgt-arm_5.2.6/include" --include_path="C:/TexasInstruments/TivaWare_C_Series-2.1.2.111" --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C123_RB1 --define=PART_TM4C123GH6PM --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="uartstdio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


