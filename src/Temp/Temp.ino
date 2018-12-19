// Example 51.1
// Motorola MC14489 with HP 5082-7415 5-digit, 7-segment LED display
// 2k0 resistor on MC14489 Rx pin
// John Boxall 2013 CC by-sa-nc
// define pins for data from Arduino to MC14489
// we treat it just like a 74HC595
int data = 16;
int clock = 4;
int enable = 0;
int output[3] = {0,0,0};
void setup()
{
 pinMode(data, OUTPUT);
 pinMode(enable, OUTPUT);
 pinMode(clock, OUTPUT);
 displayOn(); // display defaults to off at power-up
}
void displayTest1()
// displays 5.4321
{
 digitalWrite(enable, LOW); // send 3 bytes to display register. See data sheet page 9
 // you can also insert decimal or hexadecimal numbers in place of the binary numbers
 // we're using binary as you can easily match the nibbles (4-bits) against the table
 // in data sheet page 8
 output[0] = B10010101;
 output[1] = B01000011;
 output[2] = B00100001;
 shiftOut(data, clock, MSBFIRST, output[0]); // D23~D16
 shiftOut(data, clock, MSBFIRST, output[1]); // D15~D8
 shiftOut(data, clock, MSBFIRST, output[2]); // D7~D0
 digitalWrite(enable, HIGH);
 delay(10);
}
void displayTest2()
// displays ABCDE
{
 digitalWrite(enable, LOW); // send 3 bytes to display register. See data sheet page 9
 // you can also insert decimal or hexadecimal numbers in place of the binary numbers
 // we're using binary as you can easily match the nibbles (4-bits) against the table
 // in data sheet page 8
 output[0] = B10101010;
 output[1] = B10111100;
 output[2] = B11011110;
 shiftOut(data, clock, MSBFIRST, output[0]); // D23~D16
 shiftOut(data, clock, MSBFIRST, output[1]); // D15~D8
 shiftOut(data, clock, MSBFIRST, output[2]); // D7~D0
 digitalWrite(enable, HIGH);
 delay(10);
}
void displayOn()
// turns on display
{
 digitalWrite(enable, LOW);
 shiftOut(data, clock, MSBFIRST, B00000001);
 digitalWrite(enable, HIGH);
 delay(10);
}
void displayOff()
// turns off display
{
 digitalWrite(enable, LOW);
 shiftOut(data, clock, MSBFIRST, B00000000);
 digitalWrite(enable, HIGH);
 delay(10);
}

void flipoutput(){
  
  }
void loop()
{
 displayOn();
 displayTest1();
 delay(1000);
 displayTest2();
 delay(1000);
 displayOff();
 delay(500);
}
