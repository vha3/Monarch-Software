################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="/Users/hunteradams/Documents/MSP430/workspace/Monarch_Software" --include_path="/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/include" --define=DeviceFamily_CC13X0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-340650415:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-340650415-inproc

build-340650415-inproc: ../hello.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/Applications/ti/xdctools_3_50_02_20_core/xs" --xdcpath="/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/source;/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages;/Applications/ti/ccsv8/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC1310F128 -r release -c "/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS" --compileOptions "-mv7M3 --code_state=16 --float_support=vfplib -me --include_path=\"/Users/hunteradams/Documents/MSP430/workspace/Monarch_Software\" --include_path=\"/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages/ti/sysbios/posix\" --include_path=\"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/include\" --define=DeviceFamily_CC13X0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-340650415 ../hello.cfg
configPkg/compiler.opt: build-340650415
configPkg/: build-340650415


