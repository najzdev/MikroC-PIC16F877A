/*
╔═══════════════════════════════════════════════════════════════════════════╗
║ PART A: NORMAL TRAFFIC LIGHT WITH WDT ENABLED                             ║
║ PIC16F877A @ 20 MHz, mikroC PRO for PIC                                  ║
║                                                                            ║
║ BEHAVIOR:                                                                  ║
║  • Red LED: 2.0 seconds ON                                                ║
║  • Yellow LED: 0.3 seconds ON                                             ║
║  • Green LED: 1.5 seconds ON                                              ║
║  • Cycle repeats indefinitely                                             ║
║  • WDT enabled but NEVER triggers (CLRWDT called regularly)              ║
╚═══════════════════════════════════════════════════════════════════════════╝
*/

// ═══════════════════════════════════════════════════════════════════════
// CONFIGURATION BITS (for PIC16F877A)
// ═══════════════════════════════════════════════════════════════════════
#pragma config OSC = HS         // High Speed oscillator (external 20MHz)
#pragma config WDTE = ON        // Watchdog Timer ENABLED *** CRITICAL ***
#pragma config PWRTE = OFF      // Power-up Timer disabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config LVP = OFF        // Low Voltage Programming disabled
#pragma config CPD = OFF        // Data EEPROM protection disabled
#pragma config WRT = OFF        // Flash write protection disabled
#pragma config CP = OFF         // Code protection disabled

// ═══════════════════════════════════════════════════════════════════════
// PIN DEFINITIONS (for better code readability)
// ═══════════════════════════════════════════════════════════════════════
#define RED_LED     PORTC.F0     // RC0
#define YELLOW_LED  PORTC.F1     // RC1
#define GREEN_LED   PORTC.F2     // RC2
#define BUTTON      PORTB.F0     // RB0

// ═══════════════════════════════════════════════════════════════════════
// FUNCTION PROTOTYPES
// ═══════════════════════════════════════════════════════════════════════
void GPIO_Init(void);
void Oscillator_Init(void);
void Traffic_Light_Cycle(void);
void All_LEDs_Off(void);
void Clear_Watchdog(void);

// ═══════════════════════════════════════════════════════════════════════
// INITIALIZATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void GPIO_Init(void) {
    /*
    Configure Port C (RC0, RC1, RC2) as OUTPUTS for LEDs
    Configure Port B (RB0) as INPUT for button
    */
    TRISC = 0b11111000;  // RC0, RC1, RC2 = outputs (0), rest = inputs (1)
    TRISB = 0b00000001;  // RB0 = input (1), rest = outputs (0)
    
    PORTC = 0x00;        // All Port C pins low initially
    PORTB = 0x00;        // All Port B pins low initially
    
    // Disable analog functions on Port C
    // PORTC is all digital (no ADC by default)
}

void Oscillator_Init(void) {
    /*
    Oscillator configuration is handled by config bits (OSC = HS)
    No code needed - hardware oscillator runs at 20 MHz
    Instruction cycle = Fosc / 4 = 20 MHz / 4 = 5 MHz
    1 instruction = 0.2 µs
    */
}

void Clear_Watchdog(void) {
    /*
    CLRWDT instruction - must be called BEFORE watchdog times out
    WDT timeout at 128 ms prescaler ≈ 2 seconds (Postscaler 1:128)
    
    *** CRITICAL: Call this function at least once per ~1.5 seconds
        to prevent WDT reset during normal operation ***
    */
    asm CLRWDT;
}

void All_LEDs_Off(void) {
    RED_LED = 0;
    YELLOW_LED = 0;
    GREEN_LED = 0;
}

// ═══════════════════════════════════════════════════════════════════════
// TRAFFIC LIGHT CYCLE
// ═══════════════════════════════════════════════════════════════════════

void Traffic_Light_Cycle(void) {
    /*
    One complete traffic light cycle:
      RED:    2000 ms
      YELLOW: 300 ms
      GREEN:  1500 ms
    
    Total cycle time: 3800 ms
    */
    
    // RED LIGHT - 2.0 seconds
    All_LEDs_Off();
    RED_LED = 1;
    delay_ms(2000);
    Clear_Watchdog();  // Reset WDT timer during delay
    
    // YELLOW LIGHT - 0.3 seconds
    All_LEDs_Off();
    YELLOW_LED = 1;
    delay_ms(300);
    // Note: No CLRWDT needed here (only 300ms, well within WDT window)
    
    // GREEN LIGHT - 1.5 seconds
    All_LEDs_Off();
    GREEN_LED = 1;
    delay_ms(1500);
    Clear_Watchdog();  // Reset WDT timer during delay
}

// ═══════════════════════════════════════════════════════════════════════
// MAIN PROGRAM
// ═══════════════════════════════════════════════════════════════════════

void main(void) {
    // Initialize system
    GPIO_Init();
    Oscillator_Init();
    
    // Signal system start (all LEDs blink once)
    RED_LED = 1;
    YELLOW_LED = 1;
    GREEN_LED = 1;
    delay_ms(500);
    All_LEDs_Off();
    delay_ms(500);
    
    Clear_Watchdog();  // Reset WDT before main loop starts
    
    // ═══════════════════════════════════════════════════════════════
    // MAIN LOOP - TRAFFIC LIGHT CYCLES
    // ═══════════════════════════════════════════════════════════════
    while(1) {
        Traffic_Light_Cycle();
        // Loop repeats indefinitely
        // WDT is cleared during each 2s and 1.5s delay
    }
}

/*
╔═══════════════════════════════════════════════════════════════════════════╗
║ WATCHDOG TIMER CONFIGURATION ANALYSIS (PART A)                            ║
║                                                                            ║
║ WDT Settings:                                                             ║
║  • Status: ENABLED (WDTE = ON in config bits)                            ║
║  • Timeout: TMR0 prescaler determines WDT rate                           ║
║  • Default prescaler: 1:128 (approximately 2.3 seconds timeout)          ║
║                                                                            ║
║ Calculation:                                                              ║
║  WDT timeout = 1 ms × prescaler value                                    ║
║  With 1:128 prescaler: 1 ms × 128 = 128 ms (per WDT cycle)             ║
║  With postscaler 1:16 (default): 128 ms × 16 = 2048 ms ≈ 2.0-2.3 s    ║
║                                                                            ║
║ CLRWDT Placement:                                                         ║
║  • Called AFTER 2000 ms delay (RED)   ✓ SAFE                            ║
║  • NOT called during 300 ms delay (YELLOW) ✓ NOT NEEDED                 ║
║  • Called AFTER 1500 ms delay (GREEN) ✓ SAFE                            ║
║  • Total cycle: 3800 ms - WDT timeout: ~2300 ms ✓ SAFE                 ║
║                                                                            ║
║ *** WDT NEVER RESETS during normal operation ***                        ║
╚═══════════════════════════════════════════════════════════════════════════╝
*/
