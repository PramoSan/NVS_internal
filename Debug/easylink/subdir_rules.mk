################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
easylink/%.obj: ../easylink/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/home/pramodh/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O3 --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs" --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="/home/pramodh/ti/simplelink_cc13xx_cc26xx_sdk_5_30_01_01/source" --include_path="/home/pramodh/ti/simplelink_cc13xx_cc26xx_sdk_5_30_01_01/source/ti/posix/ccs" --include_path="/home/pramodh/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=DeviceFamily_CC13X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="easylink/$(basename $(<F)).d_raw" --include_path="/home/pramodh/workspace_v11/nvsinternal_CC1352R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" --obj_directory="easylink" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


