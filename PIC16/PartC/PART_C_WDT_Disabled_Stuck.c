/*
╔═══════════════════════════════════════════════════════════════════════════╗
║ PART C: WDT DISABLED - SYSTEM GETS STUCK PERMANENTLY                      ║
║ PIC16F877A @ 20 MHz, mikroC PRO for PIC                                  ║
║                                                                            ║
║ BEHAVIOR:                                                                  ║
║  • Normal: Traffic light cycles (same as Part B)                          ║
║  • Button Pressed: Enter infinite loop (same as Part B)                   ║
║  • DIFFERENCE: WDT is DISABLED                                            ║
║  • Result: NO WDT RESET - System stuck forever                           ║
║  • Recovery: NONE - only manual power cycle restarts system               ║
║                                                                            ║
║ THIS DEMONSTRATES:                                                         ║
║  ✗ Vulnerability without WDT                                              ║
║  ✗ Why WDT is essential for reliability                                   ║
║  ✗ Consequences of software bugs with no watchdog                        ║
╚═══════════════════════════════════════════════════════════════════════════╝
*/

// ═══════════════════════════════════════════════════════════════════════
// CONFIGURATION BITS
// ═══════════════════════════════════════════════════════════════════════
#pragma config OSC = HS         // High Speed oscillator (20MHz)
#pragma config WDTE = OFF       // *** WDT DISABLED *** (CRITICAL CHANGE)
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
#define BUTTON      PORTB.F0     // RB0

// ═══════════════════════════════════════════════════════════════════════
// FUNCTION PROTOTYPES
// ═══════════════════════════════════════════════════════════════════════
void GPIO_Init(void);
void Oscillator_Init(void);
void Traffic_Light_Cycle(void);
void All_LEDs_Off(void);
void Check_Button(void);
void Failure_Infinite_Loop(void);
void Startup_Indication(void);

// ═══════════════════════════════════════════════════════════════════════
// INITIALIZATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void GPIO_Init(void) {
    TRISC = 0b11111000;
    TRISB = 0b00000001;
    PORTC = 0x00;
    PORTB = 0x00;
}

void Oscillator_Init(void) {
    // 20 MHz external oscillator
}

void All_LEDs_Off(void) {
    RED_LED = 0;
    YELLOW_LED = 0;
    GREEN_LED = 0;
}

void Startup_Indication(void) {
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
// BUTTON CHECK
// ═══════════════════════════════════════════════════════════════════════

void Check_Button(void) {
    if(BUTTON == 1) {
        Failure_Infinite_Loop();
    }
}

// ═══════════════════════════════════════════════════════════════════════
// FAILURE SIMULATION (NO WDT TO RESCUE)
// ═══════════════════════════════════════════════════════════════════════

void Failure_Infinite_Loop(void) {
    /*
    ╔═══════════════════════════════════════════════════════════════════╗
    ║ INFINITE LOOP WITHOUT WDT PROTECTION                              ║
    ║                                                                    ║
    ║ Behavior with WDT DISABLED:                                       ║
    ║  1. Enter this function when button pressed                       ║
    ║  2. Turn RED LED ON (error indication)                            ║
    ║  3. Enter infinite loop                                           ║
    ║  4. WDT is DISABLED → NO timeout after 2.3s                      ║
    ║  5. System loops FOREVER ← STUCK PERMANENTLY                      ║
    ║  6. Red LED remains ON indefinitely                               ║
    ║  7. No automatic recovery possible                                ║
    ║  8. Only solution: Manual power cycle (toggle power)              ║
    ║                                                                    ║
    ║ *** THIS IS WHY EMBEDDED SYSTEMS MUST HAVE WDT ***               ║
    ╚═══════════════════════════════════════════════════════════════════╝
    */
    
    All_LEDs_Off();
    RED_LED = 1;  // Error indication
    
    // Infinite loop (no escape possible without external reset)
    while(1) {
        // System STUCK HERE FOREVER
        // Red LED stays ON
        // No recovery mechanism available
    }
}

// ═══════════════════════════════════════════════════════════════════════
// TRAFFIC LIGHT
// ═══════════════════════════════════════════════════════════════════════

void Traffic_Light_Cycle(void) {
    // RED LIGHT - 2.0 seconds
    All_LEDs_Off();
    RED_LED = 1;
    delay_ms(2000);
    Check_Button();
    
    // YELLOW LIGHT - 0.3 seconds
    All_LEDs_Off();
    YELLOW_LED = 1;
    delay_ms(300);
    Check_Button();
    
    // GREEN LIGHT - 1.5 seconds
    All_LEDs_Off();
    GREEN_LED = 1;
    delay_ms(1500);
    Check_Button();
}

// ═══════════════════════════════════════════════════════════════════════
// MAIN PROGRAM
// ═══════════════════════════════════════════════════════════════════════

void main(void) {
    GPIO_Init();
    Oscillator_Init();
    
    Startup_Indication();
    
    // Note: No CLRWDT call here (WDT is disabled anyway)
    
    while(1) {
        Traffic_Light_Cycle();
    }
}

/*
╔═══════════════════════════════════════════════════════════════════════════╗
║ PART C EXECUTION SCENARIO                                                  ║
║                                                                            ║
║ SCENARIO: Button pressed at t=1.0s (WITH WDT DISABLED)                   ║
║ ─────────────────────────────────────────────────────────────────────────  ║
║  0s    → Startup indication                                               ║
║  500ms → RED phase begins                                                 ║
║  1.0s  → BUTTON PRESSED!                                                  ║
║  1.0s  → Check_Button() detects HIGH on RB0                               ║
║  1.0s  → Failure_Infinite_Loop() called                                   ║
║  1.0s  → RED_LED ON                                                       ║
║  1.0s  → Infinite loop begins                                             ║
║                                                                            ║
║  2.3s  → Time: WDT WOULD timeout (if enabled)                            ║
║          Status: NOTHING HAPPENS ← WDT is DISABLED!                       ║
║          System still looping...                                          ║
║                                                                            ║
║  5.0s  → Still looping...                                                 ║
║  10s   → Still looping...                                                 ║
║  1 min → Still looping...                                                 ║
║  1 hr  → Still looping...                                                 ║
║                                                                            ║
║  RED LED: STAYS ON (error frozen on display)                              ║
║                                                                            ║
║  RECOVERY OPTIONS: NONE AVAILABLE!                                        ║
║  ✗ Cannot press button again (program stuck in loop)                      ║
║  ✗ WDT cannot help (disabled)                                             ║
║  ✗ Only option: Toggle power switch OFF then ON                          ║
║                                                                            ║
║ THIS DEMONSTRATES CRITICAL IMPORTANCE OF WDT IN EMBEDDED SYSTEMS          ║
╚═══════════════════════════════════════════════════════════════════════════╝
*/
