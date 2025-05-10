#include <LiquidCrystal.h>
#include <DHT11.h>
#include <Stepper.h>

#define RDA 0x80
#define TBE 0x20

#define DHTTYPE DHT11
DHT11 dht11(6);

int THRESHOLD_T = 30;
int THRESHOLD_W = 250;
// Set up Stepper Motor using Stepper motor Library
Stepper stepper(200, 45, 43, 41, 39);

// Buttons 9, 10, 11
volatile unsigned char *port_h = (unsigned char *)0x102;
volatile unsigned char *ddr_h = (unsigned char *)0x101;
volatile unsigned char *pin_h = (unsigned char *)0x100;

// Leds, 53, 51, 49

volatile unsigned char *port_b = (unsigned char *)0x25;
volatile unsigned char *ddr_b = (unsigned char *)0x24;
volatile unsigned char *pin_b = (unsigned char *)0x23;

volatile unsigned char *port_l = (unsigned char *)0x10B;
volatile unsigned char *ddr_l = (unsigned char *)0x10A;
volatile unsigned char *pin_l = (unsigned char *)0x109;

// LCD pins <--> Arduino pins
const int RS = 11, EN = 12, D4 = 2, D5 = 3, D6 = 4, D7 = 5;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
// Serial Monitor
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int *myUBRR0 = (unsigned int *)0x00C4;
volatile unsigned char *myUDR0 = (unsigned char *)0x00C6;
// ADC - Water Sensor
volatile unsigned char *my_ADMUX = (unsigned char *)0x7C;
volatile unsigned char *my_ADCSRB = (unsigned char *)0x7B;
volatile unsigned char *my_ADCSRA = (unsigned char *)0x7A;
volatile unsigned int *my_ADC_DATA = (unsigned int *)0x78;
// Timer
volatile unsigned char *myTCCR1A = (unsigned char *)0x80;
volatile unsigned char *myTCCR1B = (unsigned char *)0x81;
volatile unsigned char *myTCCR1C = (unsigned char *)0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *)0x6F;
volatile unsigned int *myTCNT1 = (unsigned int *)0x84;
volatile unsigned char *myTIFR1 = (unsigned char *)0x36;
//

void setup() {
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();
  // Set Up LCD
  lcd.begin(16, 2); // set up number of columns and rows
  lcd.clear();

  //Set LED pins to Output
  *ddr_b |= 0x01;
  *ddr_b |= 0x04;
  *ddr_l |= 0x01;
  //Set Buttons to input
  //*ddr_b &= 0xDF;
  //*port_b &= 0xDF;
  *ddr_h &= 0xBF;
  *port_h &= 0xBF;
  // Set Temp sensor pin to input
  *ddr_h &= 0xF7;
  *port_h &= 0xF7;
  // Set Fan Motor Pin to Output
  *ddr_h |= 0x10;
}
int state = 0;
int temp = 0;
int water = 0;
int humidity = 0;
void loop() {
  // read the water sensor value by calling adc_read() and check the threshold to display the message accordingly

  if (state != 0) {
    if (millis() % 10000 == 0) {

      temp = dht11.readTemperature();
      humidity = dht11.readHumidity();
      Serial.print(humidity);
      water = adc_read(0);
      if (state == 3) {
        lcd.setCursor(0, 0);
        lcd.print("Water Level");
        lcd.setCursor(0, 1);
        lcd.print("is too low!");
        Serial.print("One Minute");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.setCursor(13, 0);
        lcd.write(temp);
        lcd.setCursor(0, 1);
        lcd.print("Humidity: ");
        lcd.setCursor(10, 0);
        lcd.write(humidity);
      }
    }
  }
  if (*pin_h & 0x40){
    state += 1;
    if (state > 3) {
      state = 0;
    }
    if (state == 3){

    }
    Serial.print(state);
    my_delay();
  }

  // State 0 Disabled
  // Led Yellow

  if (state == 0) {
    *port_l |= 0x01;
    *port_b |= 0x04;
    *port_b &= ~0x01;
    // Fan OFF
    analogWrite(7, 0);
  }
  if (state == 1) {
    //LED Green
    *port_l &= ~0x01;
    *port_b |= 0x04;
    *port_b &= ~0x01;
    // Fan OFF
    analogWrite(7, 0);

    // if (temp > THRESHOLD_T){
    //   state = 2;
    // }
    // if (water <= THRESHOLD_W){
    //   state = 3;
    // }
  }
  if (state == 2) {
    *port_l &= ~0x01;
    *port_b &= ~0x04;
    *port_b |= 0x01;
    analogWrite(7, 50);
  }
  if (state == 3) {
    *port_l |= 0x01;
    *port_b &= ~0x04;
    *port_b &= ~0x01;
    analogWrite(7, 0);

  }
  /* State 1 Idle
    if (temp > THRESHOLD_T){
      state = 2;    
    }  
    if (water <= THRESHOLD_W){
      state = 3; 
    }


  // State 2 Running
    if (temp <= THRESHOLD_T){
      state = 1;    
    }  
    if (water <= THRESHOLD_W){
      state = 3; 
    }

  // State 3 Error
  if (water > THRESHOLD_W) {
    state = 1; 
  } **/


  /* display the water sensor reading 
  cs1 = adc_read(0);
  unsigned int val = cs1 / 1000;
  unsigned int rem = cs1 % 1000;
  unsigned int val2 = rem / 100;
  unsigned int rem2 = rem % 100;
  unsigned int val3 = rem2 / 10;
  unsigned int rem3 = rem2 % 10;
  unsigned int val4 = rem3 / 1;

  unsigned char char1 = val + '0';
  unsigned char char2 = val2 + '0';
  unsigned char char3 = val3 + '0';
  unsigned char char4 = val4 + '0';

  //if the value is less than the threshold display the value on the Serial monitor
  U0putchar(char1);
  U0putchar(char2);
  U0putchar(char3);
  U0putchar(char4);
  U0putchar('\n'); 
  */
}

//Use a threshold value that works for you with your sensor. There is no fixed value as sensor's sensitivity can differ.
void adc_init()  //write your code after each commented line and follow the instruction
{
  // setup the A register
  // set bit   7 to 1 to enable the ADC
  *my_ADCSRA |= 0x80;
  // clear bit 6 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0xDF;
  // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0xEF;
  // clear bit 0-2 to 0 to set prescaler selection to slow reading
  *my_ADCSRA &= 0xF8;

  // setup the B register
  // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0xF7;

  // clear bit 2-0 to 0 to set free running mode
  *my_ADCSRB &= 0xF8;

  // setup the MUX Register
  // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX &= 0x7F;
  // set bit 6 to 1 for AVCC analog reference
  *my_ADMUX |= 0x40;
  // clear bit 5 to 0 for right adjust result
  *my_ADMUX &= 0xDF;
  // clear bit 4-0 to 0 to reset the channel and gain bits
  *my_ADMUX &= 0xF0;
}
unsigned int adc_read(unsigned char adc_channel_num)  //work with channel 0
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX &= 0xF0;

  // clear the channel selection bits (MUX 5) hint: it's not in the ADMUX register
  *my_ADCSRB &= 0xF7;

  // set the channel selection bits for channel 0
  *my_ADMUX += adc_channel_num;

  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while ((*my_ADCSRA & 0x40) != 0)
    ;
  // return the result in the ADC data register and format the data based on right justification (check the lecture slide)
  *my_ADMUX &= 0xDF;
  unsigned int val = *my_ADC_DATA & 0x03FF;
  return val;
}


void U0init(int U0baud) {
  unsigned long FCPU = 16000000;
  unsigned int tbaud;
  tbaud = (FCPU / 16 / U0baud - 1);
  // Same as (FCPU / (16 * U0baud)) - 1;
  *myUCSR0A = 0x20;
  *myUCSR0B = 0x18;
  *myUCSR0C = 0x06;
  *myUBRR0 = tbaud;
}
unsigned char U0kbhit() {
  return *myUCSR0A & RDA;
}
unsigned char U0getchar() {
  return *myUDR0;
}
void U0putchar(unsigned char U0pdata) {
  while ((*myUCSR0A & TBE) == 0)
    ;
  *myUDR0 = U0pdata;
}

void my_delay() {
  *myTCCR1B &= 0xF8;
  *myTCNT1 = (unsigned int)(65536 - 62500);
  *myTCCR1A = 0x0;
  *myTCCR1B |= 0x04;
  while ((*myTIFR1 & 0x01) == 0)
    ;
  *myTCCR1B &= 0xF8;
  *myTIFR1 |= 0x01;
}
