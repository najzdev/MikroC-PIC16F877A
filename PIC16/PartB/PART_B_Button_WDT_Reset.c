/*
╔═══════════════════════════════════════════════════════════════════════════╗
║ PART B: BUTTON-TRIGGERED WDT RESET DEMONSTRATION                          ║
║ PIC16F877A @ 20 MHz, mikroC PRO for PIC                                  ║
║                                                                            ║
║ BEHAVIOR:                                                                  ║
║  • Normal: Traffic light cycles (Red 2s → Yellow 0.3s → Green 1.5s)      ║
║  • Button Pressed: Enter infinite loop WITHOUT CLRWDT                     ║
║  • Result: WDT timeout occurs → System automatically RESETS               ║
║  • Recovery: After reset, system restarts from main()                     ║
║                                                                            ║
║ THIS DEMONSTRATES:                                                         ║
║  ✓ WDT as a safety mechanism                                              ║
║  ✓ Automatic recovery from software failures                              ║
║  ✓ Why CLRWDT must be strategically placed                               ║
╚═══════════════════════════════════════════════════════════════════════════╝
*/

// ═══════════════════════════════════════════════════════════════════════
// CONFIGURATION BITS
// ═══════════════════════════════════════════════════════════════════════
#pragma config OSC = HS         // High Speed oscillator (20MHz)
#pragma config WDTE = ON        // Watchdog Timer ENABLED ***
#pragma config PWRTE = OFF      // Power-up Timer disabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config LVP = OFF        // Low Voltage Programming disabled
#pragma config CPD = OFF        // Data EEPROM protection disabled
#pragma config WRT = OFF        // Flash write protection disabled
#pragma config CP = OFF         // Code protection disabled

// ═══════════════════════════════════════════════════════════════════════
// PIN DEFINITIONS
// ═══════════════════════════════════════════════════════════════════════
#define RED_LED     PORTC.F0     // RC0
#define YELLOW_LED  PORTC.F1     // RC1
#define GREEN_LED   PORTC.F2     // RC2
#define BUTTON      PORTB.F0     // RB0 - INPUT

// ═══════════════════════════════════════════════════════════════════════
// FUNCTION PROTOTYPES
// ═══════════════════════════════════════════════════════════════════════
void GPIO_Init(void);
void Oscillator_Init(void);
void Traffic_Light_Cycle(void);
void All_LEDs_Off(void);
void Clear_Watchdog(void);
void Check_Button(void);
void Failure_Infinite_Loop(void);
void Startup_Indication(void);

// ═══════════════════════════════════════════════════════════════════════
// INITIALIZATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void GPIO_Init(void) {
    TRISC = 0b11111000;  // RC0, RC1, RC2 = outputs
    TRISB = 0b00000001;  // RB0 = input (button), rest = outputs
    
    PORTC = 0x00;
    PORTB = 0x00;
}

void Oscillator_Init(void) {
    // 20 MHz external oscillator via config bits
}

void Clear_Watchdog(void) {
    asm CLRWDT;
}

void All_LEDs_Off(void) {
    RED_LED = 0;
    YELLOW_LED = 0;
    GREEN_LED = 0;
}

void Startup_Indication(void) {
    /*
    Blink all LEDs 3 times to indicate system startup
    Helps distinguish reset from initial power-on
    */
    unsigned char i;
    for(i = 0; i < 3; i++) {
        RED_LED = 1;
        YELLOW_LED = 1;
        GREEN_LED = 1;
        delay_ms(200);
        All_LEDs_Off();
        delay_ms(200);
    }
    delay_ms(500);
}

// ═══════════════════════════════════════════════════════════════════════
// BUTTON CHECK FUNCTION
// ═══════════════════════════════════════════════════════════════════════

void Check_Button(void) {
    /*
    Check if button is pressed (RB0 = HIGH)
    If pressed, enter failure loop (demonstrates WDT reset)
    */
    if(BUTTON == 1) {  // Button pressed (pulled high)
        Failure_Infinite_Loop();
    }
}

// ═══════════════════════════════════════════════════════════════════════
// FAILURE SIMULATION FUNCTION
// ═══════════════════════════════════════════════════════════════════════

void Failure_Infinite_Loop(void) {
    /*
    ╔═══════════════════════════════════════════════════════════════════╗
    ║ INTENTIONAL INFINITE LOOP (simulates software failure)             ║
    ║                                                                    ║
    ║ Behavior:                                                          ║
    ║  1. Enter this function when button pressed                       ║
    ║  2. Turn RED LED ON (indicates error state)                       ║
    ║  3. Loop forever WITHOUT calling CLRWDT                          ║
    ║  4. WDT counter counts down: 2.3s → 1.0s → 0s                    ║
    ║  5. At 0s: WDT times out → RESET signal generated                ║
    ║  6. Microcontroller resets → PC jumps to 0x0000 (start of main)  ║
    ║  7. System restarts automatically ✓                               ║
    ║                                                                    ║
    ║ THIS IS EXACTLY WHY WDT IS CRITICAL FOR RELIABILITY              ║
    ╚═══════════════════════════════════════════════════════════════════╝
    */
    
    All_LEDs_Off();
    RED_LED = 1;  // Error indication: RED LED stays ON
    
    // *** INTENTIONAL: NO CLRWDT CALLED IN THIS LOOP ***
    // WDT counter will expire after ~2.3 seconds
    while(1) {
        // Infinite loop - simulates stuck software
        // WDT timeout inevitable
    }
    
    // This line never executes (infinite loop above)
}

// ═══════════════════════════════════════════════════════════════════════
// TRAFFIC LIGHT FUNCTION
// ═══════════════════════════════════════════════════════════════════════

void Traffic_Light_Cycle(void) {
    /*
    Normal traffic light cycle with button checking
    */
    
    // RED LIGHT - 2.0 seconds
    All_LEDs_Off();
    RED_LED = 1;
    delay_ms(2000);
    Check_Button();        // Check if button pressed
    Clear_Watchdog();      // Reset WDT (normal operation)
    
    // YELLOW LIGHT - 0.3 seconds
    All_LEDs_Off();
    YELLOW_LED = 1;
    delay_ms(300);
    Check_Button();        // Check if button pressed
    
    // GREEN LIGHT - 1.5 seconds
    All_LEDs_Off();
    GREEN_LED = 1;
    delay_ms(1500);
    Check_Button();        // Check if button pressed
    Clear_Watchdog();      // Reset WDT (normal operation)
}

// ═══════════════════════════════════════════════════════════════════════
// MAIN PROGRAM
// ═══════════════════════════════════════════════════════════════════════

void main(void) {
    GPIO_Init();
    Oscillator_Init();
    
    // Startup indication (3 blinks)
    Startup_Indication();
    
    Clear_Watchdog();
    
    // ═══════════════════════════════════════════════════════════════
    // MAIN LOOP
    // ═══════════════════════════════════════════════════════════════
    while(1) {
        Traffic_Light_Cycle();
    }
}

/*
╔═══════════════════════════════════════════════════════════════════════════╗
║ PART B EXECUTION SCENARIO                                                  ║
║                                                                            ║
║ SCENARIO 1: Normal operation (Button NOT pressed)                         ║
║ ─────────────────────────────────────────────────────────────────────     ║
║  0s   → Startup indication (3 blinks)                                     ║
║  2s   → RED, button checked (LOW), CLRWDT called                          ║
║  2.3s → YELLOW, button checked (LOW)                                      ║
║  3.8s → GREEN, button checked (LOW), CLRWDT called                        ║
║  [Cycle repeats indefinitely] ✓                                           ║
║                                                                            ║
║ SCENARIO 2: Button pressed at t=1.0s                                      ║
║ ─────────────────────────────────────────────────────────────────────     ║
║  0s    → Startup indication, RED begins                                   ║
║  1.0s  → Button pressed! Check_Button() detects HIGH on RB0               ║
║  1.0s  → Failure_Infinite_Loop() called                                   ║
║  1.0s  → RED_LED turned ON (already ON)                                   ║
║  1.0s  → Infinite loop begins (NO CLRWDT called)                          ║
║                                                                            ║
║  WDT Counter Timeline:                                                     ║
║  1.0s → WDT timeout ≈ 2.3s remaining                                      ║
║  2.0s → WDT timeout ≈ 1.3s remaining                                      ║
║  3.0s → WDT timeout ≈ 0.3s remaining                                      ║
║  3.3s → WDT TIMEOUT! Counter = 0                                          ║
║                                                                            ║
║  At t=3.3s: WDT triggers RESET signal                                     ║
║  → Microcontroller RESET (same as power-up reset)                         ║
║  → Program counter jumps to 0x0000 (start of main)                        ║
║  → Startup indication plays again (confirms reset)                        ║
║  → System recovers autonomously ✓✓✓                                       ║
║                                                                            ║
║ THIS DEMONSTRATES WDT'S CRITICAL SAFETY FUNCTION                          ║
╚═══════════════════════════════════════════════════════════════════════════╝
*/
