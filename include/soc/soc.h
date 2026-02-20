#pragma once
// esp32emu: soc/soc.h stub — common SoC-level defines

#define DR_REG_RTCCNTL_BASE  0x3FF48000
#define DR_REG_IO_MUX_BASE   0x3FF49000
#define DR_REG_GPIO_BASE     0x3FF44000

#define REG_READ(addr)       (0)
#define REG_WRITE(addr, val) ((void)(addr), (void)(val))
#define REG_SET_BIT(addr, b) ((void)(addr), (void)(b))
#define REG_CLR_BIT(addr, b) ((void)(addr), (void)(b))

#define SOC_GPIO_PIN_COUNT   40
#define SOC_ADC_CHANNEL_NUM  10
