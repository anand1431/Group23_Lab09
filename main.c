#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>

// SysTick Register Definitions
#define STCTRL *((volatile uint32_t *) 0xE000E010)        // Control and Status Register
#define STRELOAD *((volatile uint32_t *) 0xE000E014)      // Reload Value Register
#define STCURRENT *((volatile uint32_t *) 0xE000E018)     // Current Value Register

// SysTick Control Bits
#define ENABLE (1 << 0)                                   // Enable SysTick timer
#define CLKINT (1 << 2)                                   // Use system clock
#define CLOCK_HZ 16000000                                 // System clock frequency in Hz
#define SYSTICK_RELOAD(us) ((CLOCK_HZ / 1000000) * (us) - 1) // Calculates reload value for given microseconds