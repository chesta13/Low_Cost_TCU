################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
mock/%.o: ../mock/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2030/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/Admin/workspace_ccstheia/TCU" -I"C:/Users/Admin/workspace_ccstheia/TCU/Debug" -I"C:/Users/Admin/workspace_ccstheia/TCU/core" -I"C:/ti/mspm0_sdk_2_06_00_05/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_06_00_05/source" -I"C:/Users/Admin/workspace_ccstheia/TCU/middleware" -I"C:/Users/Admin/workspace_ccstheia/TCU/drivers" -gdwarf-3 -MMD -MP -MF"mock/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

mock/mock_modem.o: ../mock/mock_modem.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs2030/ccs/tools/compiler/ti-cgt-armllvm_4.0.3.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/Admin/workspace_ccstheia/TCU" -I"C:/Users/Admin/workspace_ccstheia/TCU/Debug" -I"C:/ti/mspm0_sdk_2_06_00_05/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_06_00_05/source" -I"C:/Users/Admin/workspace_ccstheia/TCU/drivers" -I"C:/Users/Admin/workspace_ccstheia/TCU/middleware" -I"C:/Users/Admin/workspace_ccstheia/TCU/mock" -I"C:/Users/Admin/workspace_ccstheia/TCU/core" -gdwarf-3 -MMD -MP -MF"mock/$(basename $(<F)).d_raw" -MT"mock/$(basename\ $(<F)).o"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


