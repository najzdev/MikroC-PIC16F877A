// 7-segment (Common Cathode)
unsigned char segments[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                            0x6D, 0x7D, 0x07, 0x7F, 0x6F};

unsigned int sec_count = 0;
unsigned char digit = 0;

void interrupt() {
    if (INTCON.T0IF) {

        TMR0 = 61;          // Reload for ~10ms
        INTCON.T0IF = 0;    // Clear flag FIRST (best practice)

        sec_count++;

        if (sec_count >= 100) {   // 1 second
            digit++;

            if (digit > 9)
                digit = 0;

            PORTC = segments[digit];

            sec_count = 0;
        }
    }
}

void main() {
    ADCON1 = 0x06;

    TRISC = 0x00;
    PORTC = segments[0];

    // Timer0 config: internal clock, prescaler 1:256
    OPTION_REG = 0b00000111;

    TMR0 = 61;

    INTCON.T0IF = 0;
    INTCON.T0IE = 1;
    INTCON.GIE  = 1;

    while(1) {
        // Idle loop
    }
}