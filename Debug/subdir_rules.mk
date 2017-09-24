################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
CC1310_LAUNCHXL.obj: ../CC1310_LAUNCHXL.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="/Users/hunteradams/Documents/MSP430/workspace/Sprite" --include_path="/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=DeviceFamily_CC13X0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="CC1310_LAUNCHXL.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ccfg.obj: ../ccfg.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="/Users/hunteradams/Documents/MSP430/workspace/Sprite" --include_path="/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=DeviceFamily_CC13X0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="ccfg.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

build-1196772191:
	@$(MAKE) -Onone -f subdir_rules.mk build-1196772191-inproc

build-1196772191-inproc: ../hello.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"/Applications/xdctools_3_50_02_20_core/xs" --xdcpath="/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/source;/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages;/Applications/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC1310F128 -r release -c "/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS" --compileOptions "-mv7M3 --code_state=16 --float_support=vfplib -me --include_path=\"/Users/hunteradams/Documents/MSP430/workspace/Sprite\" --include_path=\"/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages/ti/sysbios/posix\" --include_path=\"/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include\" --define=DeviceFamily_CC13X0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-1196772191 ../hello.cfg
configPkg/compiler.opt: build-1196772191
configPkg/: build-1196772191

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="/Users/hunteradams/Documents/MSP430/workspace/Sprite" --include_path="/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ccsv7/tools/compiler/ti-cgt-arm_16.9.3.LTS/include" --define=DeviceFamily_CC13X0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


