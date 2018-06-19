################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_cache.c \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_clocks.c \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_feature.c \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_fmi_R7FS7G27H3A01CFC.c \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_group_irq.c \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_hw_locks.c \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_module_stop.c \
../synergy/ssp/src/bsp/mcu/s7g2/bsp_rom_registers.c 

OBJS += \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_cache.o \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_clocks.o \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_feature.o \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_fmi_R7FS7G27H3A01CFC.o \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_group_irq.o \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_hw_locks.o \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_module_stop.o \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_rom_registers.o 

C_DEPS += \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_cache.d \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_clocks.d \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_feature.d \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_fmi_R7FS7G27H3A01CFC.d \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_group_irq.d \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_hw_locks.d \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_module_stop.d \
./synergy/ssp/src/bsp/mcu/s7g2/bsp_rom_registers.d 


# Each subdirectory must supply rules for building sources it contributes
synergy/ssp/src/bsp/mcu/s7g2/%.o: ../synergy/ssp/src/bsp/mcu/s7g2/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\Synergy\e2studio_v6.2.0_ssp_v1.4.0\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wuninitialized -Wall -Wextra -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\api\api_changed" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\driver" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src\synergy_gen" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\src\framework\el\tx" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src\synergy_gen" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\driver" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\src\framework\el\tx" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


