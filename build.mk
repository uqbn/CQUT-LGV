# C defines
# C includes
C_INCLUDES += -Ibsp -Iapp
# C sources
C_SOURCES += $(wildcard bsp/*.c) $(wildcard app/*.c)
# AS defines
AS_DEFS +=
# ASM sources
ASM_SOURCES +=
# link flags
LDFLAGS += -u_printf_float
