################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/LCD/LCD_graphics.c \
../Sources/LCD/LCD_text.c \
../Sources/LCD/ST7789.c \
../Sources/LCD/lucida_12x19.c \
../Sources/LCD/lucida_20x31.c \
../Sources/LCD/lucida_8x13.c \
../Sources/LCD/touchscreen.c 

OBJS += \
./Sources/LCD/LCD_graphics.o \
./Sources/LCD/LCD_text.o \
./Sources/LCD/ST7789.o \
./Sources/LCD/lucida_12x19.o \
./Sources/LCD/lucida_20x31.o \
./Sources/LCD/lucida_8x13.o \
./Sources/LCD/touchscreen.o 

C_DEPS += \
./Sources/LCD/LCD_graphics.d \
./Sources/LCD/LCD_text.d \
./Sources/LCD/ST7789.d \
./Sources/LCD/lucida_12x19.d \
./Sources/LCD/lucida_20x31.d \
./Sources/LCD/lucida_8x13.d \
./Sources/LCD/touchscreen.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/LCD/%.o: ../Sources/LCD/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -flto  -g3 -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Static_Code/PDD" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Sources/LCD" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Static_Code/IO_Map" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Sources" -I"C:/Users/Michael/Documents/GitHub/MCUX_workspace/MCUX_PE_KL25Z_FRTOS_ShieldwFatFS/Generated_Code" -std=c99 -fstack-usage -Wstack-usage=256 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


