// Tableau de décodage pour 7 segments (Cathode Commune)
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
unsigned char segment_code[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

unsigned int adc_val;
unsigned short temp, dizaines, unites;

void main() {
    TRISA = 0x01;  // RA0 en entrée, RA1-RA2 en sortie
    TRISB = 0x00;  // PORTB en sortie pour les segments
    TRISC = 0x00;  // Toujours pour les 8 LEDs (Partie B)
    TRISD = 0x00;  // Pour RD0-RD1, Alarme RD7 et Buzzer RD6

    ADCON1 = 0x8E; // Configuration ADC

    while(1) {
        adc_val = ADC_Read(0);
        
        // --- Calcul de la température ---
        temp = (adc_val * 500) / 1023;
        
        dizaines = temp / 10;
        unites = temp % 10;

        // --- Affichage des LEDs et Alarme (Gardé des étapes précédentes) ---
        PORTC = adc_val;
        PORTD.B7 = (temp > 60) ? 1 : 0;
        PORTD.B6 = (temp > 60) ? 1 : 0;

        // --- Multiplexage de l'afficheur ---
        // Affichage des Dizaines
        PORTB = segment_code[dizaines];
        PORTA.B1 = 1;      // Activer Digit 1
        Delay_ms(5);       // Pause courte
        PORTA.B1 = 0;      // Désactiver Digit 1

        // Affichage des Unités
        PORTB = segment_code[unites];
        PORTA.B2 = 1;      // Activer Digit 2
        Delay_ms(5);       // Pause courte
        PORTA.B2 = 0;      // Désactiver Digit 2
    }
}