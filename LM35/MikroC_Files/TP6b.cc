void main() {
  TRISA.B0 = 1;         // Pin RA0 as input (Analog)
  TRISC = 0x00;         // All PORTC as output
  TRISD.B0 = 0;         // RD0 as output
  TRISD.B1 = 0;         // RD1 as output
  
  PORTC = 0;            // Reset outputs
  PORTD = 0;
  
  // Configuration Registers
  ADCON1 = 0b10001110;  // Right aligned, AN0 is Analog, others Digital
  ADCON0 = 0b10000001;  // Fosc/32, Channel 0 (AN0), ADC ON
  
  while(1) {
    Delay_us(25);       // Acquisition time
    GO_DONE_bit = 1;    // Start conversion
    while(GO_DONE_bit); // Wait for conversion to finish
    
    PORTC = ADRESL;      // Send lower 8 bits to PORTC
    PORTD.B0 = ADRESH.B0; // Send 9th bit to RD0
    PORTD.B1 = ADRESH.B1; // Send 10th bit to RD1
  }
}