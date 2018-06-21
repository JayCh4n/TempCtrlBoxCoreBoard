################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../delay.c \
../dgus.c \
../eeprom.c \
../gpio.c \
../main.c \
../timer.c \
../usart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
delay.o \
dgus.o \
eeprom.o \
gpio.o \
main.o \
timer.o \
usart.o

OBJS_AS_ARGS +=  \
delay.o \
dgus.o \
eeprom.o \
gpio.o \
main.o \
timer.o \
usart.o

C_DEPS +=  \
delay.d \
dgus.d \
eeprom.d \
gpio.d \
main.d \
timer.d \
usart.d

C_DEPS_AS_ARGS +=  \
delay.d \
dgus.d \
eeprom.d \
gpio.d \
main.d \
timer.d \
usart.d

OUTPUT_FILE_PATH +=TempCtrlBoxCoreBoard.elf

OUTPUT_FILE_PATH_AS_ARGS +=TempCtrlBoxCoreBoard.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)D:\AtmelStudio7\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"D:\AtmelStudio7\7.0\Packs\atmel\ATmega_DFP\1.2.150\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega64a -B "D:\AtmelStudio7\7.0\Packs\atmel\ATmega_DFP\1.2.150\gcc\dev\atmega64a" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 5.4.0
	$(QUOTE)D:\AtmelStudio7\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="TempCtrlBoxCoreBoard.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega64a -B "D:\AtmelStudio7\7.0\Packs\atmel\ATmega_DFP\1.2.150\gcc\dev\atmega64a"  
	@echo Finished building target: $@
	"D:\AtmelStudio7\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "TempCtrlBoxCoreBoard.elf" "TempCtrlBoxCoreBoard.hex"
	"D:\AtmelStudio7\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "TempCtrlBoxCoreBoard.elf" "TempCtrlBoxCoreBoard.eep" || exit 0
	"D:\AtmelStudio7\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "TempCtrlBoxCoreBoard.elf" > "TempCtrlBoxCoreBoard.lss"
	"D:\AtmelStudio7\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "TempCtrlBoxCoreBoard.elf" "TempCtrlBoxCoreBoard.srec"
	"D:\AtmelStudio7\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "TempCtrlBoxCoreBoard.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "TempCtrlBoxCoreBoard.elf" "TempCtrlBoxCoreBoard.a" "TempCtrlBoxCoreBoard.hex" "TempCtrlBoxCoreBoard.lss" "TempCtrlBoxCoreBoard.eep" "TempCtrlBoxCoreBoard.map" "TempCtrlBoxCoreBoard.srec" "TempCtrlBoxCoreBoard.usersignatures"
	