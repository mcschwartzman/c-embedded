################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.5/bin/cl430" -vmspx --abi=coffabi --data_model=large -Ooff -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/driverlib" --include_path="R:/ccs/ece2049c17_lab3_remiles_mcschwartzman" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/grlib/grlib" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/grlib/fonts" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/driverlib/MSP430F5xx_6xx" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.5/include" --advice:power_severity=suppress --gcc --define=__MSP430F5529__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

peripherals.obj: ../peripherals.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.5/bin/cl430" -vmspx --abi=coffabi --data_model=large -Ooff -g --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/driverlib" --include_path="R:/ccs/ece2049c17_lab3_remiles_mcschwartzman" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/grlib/grlib" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/grlib/fonts" --include_path="C:/ti/ccsv5/ccs_base/msp430/msp430ware_1_40_00_26/driverlib/MSP430F5xx_6xx" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.5/include" --advice:power_severity=suppress --gcc --define=__MSP430F5529__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="peripherals.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


