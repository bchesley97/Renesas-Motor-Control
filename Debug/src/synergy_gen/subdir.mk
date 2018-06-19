################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/synergy_gen/common_data.c \
../src/synergy_gen/hal_data.c \
../src/synergy_gen/main.c \
../src/synergy_gen/motion_thread.c \
../src/synergy_gen/pin_data.c \
../src/synergy_gen/pwm_ctrl_thread.c \
../src/synergy_gen/uart_thread.c 

OBJS += \
./src/synergy_gen/common_data.o \
./src/synergy_gen/hal_data.o \
./src/synergy_gen/main.o \
./src/synergy_gen/motion_thread.o \
./src/synergy_gen/pin_data.o \
./src/synergy_gen/pwm_ctrl_thread.o \
./src/synergy_gen/uart_thread.o 

C_DEPS += \
./src/synergy_gen/common_data.d \
./src/synergy_gen/hal_data.d \
./src/synergy_gen/main.d \
./src/synergy_gen/motion_thread.d \
./src/synergy_gen/pin_data.d \
./src/synergy_gen/pwm_ctrl_thread.d \
./src/synergy_gen/uart_thread.d 


# Each subdirectory must supply rules for building sources it contributes
src/synergy_gen/%.o: ../src/synergy_gen/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\Synergy\e2studio_v6.2.0_ssp_v1.4.0\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wuninitialized -Wall -Wextra -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\api\api_changed" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\driver" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src\synergy_gen" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\src\framework\el\tx" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src\synergy_gen" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\driver" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\el" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy_cfg\ssp_cfg\framework" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\framework\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\src\framework\el\tx" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\api" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\synergy\ssp\inc\driver\instances" -I"C:\Users\britton.chesley\e2_studio\workspace\S7G\src" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


