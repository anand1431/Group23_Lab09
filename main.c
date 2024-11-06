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


// Configure SysTick for delays
void systick_config(void) {
    STRELOAD = SYSTICK_RELOAD(1000);                      // Set reload for 1 ms intervals
    STCTRL |= ENABLE | CLKINT;                            // Enable SysTick with system clock
    STCURRENT = 0;                                        // Clear current value register
}

// Initialize I2C0 for communication
void I2C0_init(void) {
    SYSCTL_RCGCI2C_R |= 0x01;                             // Enable clock for I2C0 module
    SYSCTL_RCGCGPIO_R |= 0x02;                            // Enable clock for Port B

    while ((SYSCTL_PRGPIO_R & 0x02) == 0);                // Wait until Port B is ready

    GPIO_PORTB_AFSEL_R |= 0x0C;                           // Enable alternate function for PB2, PB3
    GPIO_PORTB_ODR_R |= 0x08;                             // Set PB3 (SDA) as open-drain
    GPIO_PORTB_DEN_R |= 0x0C;                             // Enable digital function on PB2, PB3
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xFF00) | 0x3300; // Assign I2C function to PB2, PB3

    I2C0_MCR_R = 0x10;                                    // Configure I2C0 as master
    I2C0_MTPR_R = 0x07;                                   // Set I2C clock frequency
}

// Send data over I2C0
void I2C0_send(uint8_t address, uint8_t msb, uint8_t lsb) {
    I2C0_MSA_R = (address << 1);                          // Set slave address and write mode
    I2C0_MDR_R = msb;                                     // Send the MSB
    I2C0_MCS_R = 0x03;                                    // Start and send
    while (I2C0_MCS_R & 0x01);                            // Wait for transfer to complete
    if (I2C0_MCS_R & 0x02) return;                        // If an error occurs, exit

    I2C0_MDR_R = lsb;                                     // Send the LSB
    I2C0_MCS_R = 0x05;                                    // Send and stop
    while (I2C0_MCS_R & 0x01);                            // Wait for transfer to complete
    if (I2C0_MCS_R & 0x02) return;                        // If an error occurs, exit
}

// Generate delay in microseconds
void delay_us(int us) {
    STRELOAD = SYSTICK_RELOAD(us);                        // Load value for the specified delay
    STCURRENT = 0;                                        // Clear current value register
    STCTRL |= ENABLE | CLKINT;                            // Start SysTick
    while ((STCTRL & (1 << 16)) == 0);                    // Wait for the count flag
    STCTRL &= ~ENABLE;                                    // Disable SysTick
}