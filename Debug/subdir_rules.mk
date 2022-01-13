################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/pramodh/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O3 --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs" --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="/home/pramodh/ti/simplelink_cc13xx_cc26xx_sdk_5_30_01_01/source" --include_path="/home/pramodh/ti/simplelink_cc13xx_cc26xx_sdk_5_30_01_01/source/ti/posix/ccs" --include_path="/home/pramodh/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=DeviceFamily_CC13X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-2000903025: ../nvsinternal.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/home/pramodh/ti/ccs1100/ccs/utils/sysconfig_1.10.0/sysconfig_cli.sh" -s "/home/pramodh/ti/simplelink_cc13xx_cc26xx_sdk_5_30_01_01/.metadata/product.json" --script "/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs/nvsinternal.syscfg" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_devices_config.c: build-2000903025 ../nvsinternal.syscfg
syscfg/ti_drivers_config.c: build-2000903025
syscfg/ti_drivers_config.h: build-2000903025
syscfg/ti_utils_build_linker.cmd.genlibs: build-2000903025
syscfg/syscfg_c.rov.xs: build-2000903025
syscfg/ti_utils_runtime_model.gv: build-2000903025
syscfg/ti_utils_runtime_Makefile: build-2000903025
syscfg/: build-2000903025

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/pramodh/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O3 --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs" --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="/home/pramodh/ti/simplelink_cc13xx_cc26xx_sdk_5_30_01_01/source" --include_path="/home/pramodh/ti/simplelink_cc13xx_cc26xx_sdk_5_30_01_01/source/ti/posix/ccs" --include_path="/home/pramodh/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=DeviceFamily_CC13X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


