################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../hello.cfg 

CMD_SRCS += \
../CC1310_LAUNCHXL_TIRTOS.cmd 

C_SRCS += \
../CC1310_LAUNCHXL.c \
../ccfg.c \
../main.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./CC1310_LAUNCHXL.d \
./ccfg.d \
./main.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./CC1310_LAUNCHXL.obj \
./ccfg.obj \
./main.obj 

GEN_MISC_DIRS__QUOTED += \
"configPkg/" 

OBJS__QUOTED += \
"CC1310_LAUNCHXL.obj" \
"ccfg.obj" \
"main.obj" 

C_DEPS__QUOTED += \
"CC1310_LAUNCHXL.d" \
"ccfg.d" \
"main.d" 

GEN_FILES__QUOTED += \
"configPkg/linker.cmd" \
"configPkg/compiler.opt" 

C_SRCS__QUOTED += \
"../CC1310_LAUNCHXL.c" \
"../ccfg.c" \
"../main.c" 


