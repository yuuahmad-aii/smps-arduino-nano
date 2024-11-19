#include <avr/io.h>

#define F_CPU 16000000UL

void initPWM()
{
  // Konfigurasi pin PWM (PB1/Pin 9 dan PB2/Pin 10)
  DDRB |= (1 << PB1) | (1 << PB2); // Atur PB1 dan PB2 sebagai output (OC1A dan OC1B)

  // Konfigurasi Timer1 untuk Fast PWM, mode 14 (TOP = ICR1)
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // Non-inverting mode, Fast PWM
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);    // Fast PWM, prescaler = 1

  // Hitung nilai TOP untuk frekuensi 50 kHz
  ICR1 = (F_CPU / 50000) - 1; // TOP = (16 MHz / 50 kHz) - 1 = 319
  OCR1A = 0;                  // Mulai dengan duty cycle 0% untuk channel A
  OCR1B = 0;                  // Mulai dengan duty cycle 0% untuk channel B
}

void initADC()
{
  // Konfigurasi ADC untuk membaca potensiometer
  ADMUX = (1 << REFS0);   // Gunakan AVcc sebagai referensi (5V)
  ADCSRA = (1 << ADEN)    // Aktifkan ADC
           | (1 << ADPS2) // Prescaler = 64 (Clock ADC = 250 kHz)
           | (1 << ADPS1);
}

uint16_t readADC(uint8_t channel)
{
  // Pilih channel ADC (0–7)
  ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // Pilih channel dengan menjaga bit 3-7

  // Mulai konversi ADC
  ADCSRA |= (1 << ADSC);

  // Tunggu hingga konversi selesai
  while (ADCSRA & (1 << ADSC))
    ;

  // Kembalikan hasil konversi ADC
  return ADC;
}

int main(void)
{
  initPWM(); // Inisialisasi PWM
  initADC(); // Inisialisasi ADC

  while (1)
  {
    // Baca nilai potensiometer dari channel ADC0 dan ADC1
    uint16_t potValueA = readADC(0); // Input ADC0 untuk output PWM pada OC1A
    uint16_t potValueB = readADC(1); // Input ADC1 untuk output PWM pada OC1B

    // Konversi nilai potensiometer (0–1023) ke duty cycle (0–100%)
    // uint16_t dutyCycleB = (potValueB * 100) / 1023;
    // uint16_t dutyCycleA = (potValueA * 100) / 1023;

    // Atur OCR1A dan OCR1B untuk mengubah duty cycle PWM
    OCR1A = potValueA; // Channel A (OC1A)
    OCR1B = potValueB; // Channel B (OC1B)
  }

  return 0;
}
