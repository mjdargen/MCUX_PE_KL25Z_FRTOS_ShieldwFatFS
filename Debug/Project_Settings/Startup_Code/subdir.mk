################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Project_Settings/Startup_Code/startup.c 

OBJS += \
./Project_Settings/Startup_Code/startup.o 

C_DEPS += \
./Project_Settings/Startup_Code/startup.d 


# Each subdirectory must supply rules for building sources it contributes
Project_Settings/Startup_Code/%.o: ../Project_Settings/Startup_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto  -g3 -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Static_Code/PDD" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Sources/LCD" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Static_Code/IO_Map" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Sources" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Generated_Code" -std=c99 -fstack-usage -Wstack-usage=256 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


