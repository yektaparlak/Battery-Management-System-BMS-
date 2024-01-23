#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize two instances of the LiquidCrystal_I2C library for two LCD displays
LiquidCrystal_I2C ekran1(0x27, 20, 4);
LiquidCrystal_I2C ekran2(0x26, 20, 4);

// Declare variables for cell voltages
float Vs1, Vs2, Vs3, Vs4, Vs5, Vs6, Vs7, Vs8, Vs9, Vs10, Vs11;

// Declare variables for filtered cell voltages
float V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11;

// Declare variables for the sum of cell voltages
float Sum1, Sum2, Sum3, Sum4, Sum5, Sum6, Sum7, Sum8, Sum9, Sum10, Sum11;

// Declare variables for average cell voltages
float Av1, Av2, Av3, Av4, Av5, Av6, Av7, Av8, Av9, Av10, Av11;

// Declare variables for individual cell voltages
float Cell1, Cell2, Cell3, Cell4, Cell5;

// Declare resistor values for voltage divider
float r1 = 10000.0;
float r2 = 1000.0;

// Declare minimum voltage
float Vmin = 0;

// Declare variable for ampere value
int AmpValue;

// Declare reference voltage and tolerance
float refVolt = 0;
float tol = 0.2;

// Declare variables for voltage differences between cells
float dif1 = 0;
float dif2 = 0;
float dif3 = 0;
float dif4 = 0;

// Declare variables for balancing status of each cell
int bal1 = 0;
int bal2 = 0;
int bal3 = 0;
int bal4 = 0;
int bal5 = 0;

// Declare balancing and protection voltage thresholds
float BalanceVal = 3.65;
float ProtectV = 3.69;
float DischVolt = 2.00;

// Declare pin numbers for relays and balancing MOSFETs
int RelayPro = 2;
int BalMos1 = 9;
int BalMos2 = 8;
int BalMos3 = 5;
int BalMos4 = 4;
int BalMos5 = 3;

// Declare variables for system state
int block = 0;
int lock = 0;

// Declare pin numbers for thermistors
int Thermistor1Pin = A10;
int Thermistor2Pin = A11;
int Thermistor3Pin = A12;
int Thermistor4Pin = A13;
int Thermistor5Pin = A14;

// Declare variables for thermistor temperatures
float TF1, TF2, TF3, TF4, TF5;
float temp1, temp2, temp3, temp4, temp5;

// Declare maximum temperature threshold
int maxtemp = 60;

// Declare variables for temperature calculations using thermistors
int Ko;
float R1 = 10000;
float logR2, R2, T1, T2, T3, T4, T5, Tc1, Tc2, Tc3, Tc4, Tc5;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// Setup function: initializes serial communication, LCD displays, pins, and libraries
void setup() {
  Serial.begin(9600);
  ekran1.begin();
  ekran2.begin();

  // Set analog pins as inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);

  // Set thermistor pins as inputs
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);

  // Set balancing MOSFET pins as outputs
  pinMode(BalMos1, OUTPUT);
  pinMode(BalMos2, OUTPUT);
  pinMode(BalMos3, OUTPUT);
  pinMode(BalMos4, OUTPUT);
  pinMode(BalMos5, OUTPUT);

  // Set relay pin as output
  pinMode(RelayPro, OUTPUT);
}



void loop() {
  // Print average cell voltages to serial monitor
  Serial.print("1 = ");
  Serial.println(Av1);
  Serial.print("2 = ");
  Serial.println(Av2);
  Serial.print("3 = ");
  Serial.println(Av3);
  Serial.print("4 = ");
  Serial.println(Av4);
  Serial.print("5 = ");
  Serial.println(Av5);
  Serial.print("6 = ");
  Serial.println(Av6);
  Serial.print("7 = ");
  Serial.println(Av7);
  Serial.print("8 = ");
  Serial.println(Av8);
  Serial.print("9 = ");
  Serial.println(Av9);
  Serial.print("10 = ");
  Serial.println(Av10);

  // Current Sense
  Sum11 = 0;

  // Read analog value from A15 and calculate voltage
  for (int i = 0; i < 150; i++) {
    int analogvalue = analogRead(A15);
    V11 = ((analogvalue * 4.82) / 1024);
    delay(5);
    Sum11 = Sum11 + V11;
  }
  Av11 = (Sum11 / 150);

  // Display current on LCD
  if ((Av11 >= 0.00) && (Av11 <= 2.45)) {
    ekran2.setCursor(10, 2);
    ekran2.print("IL:");
    ekran2.setCursor(13, 2);
    ekran2.print("       ");
    ekran2.setCursor(13, 2);
    ekran2.print("0A   ");
  } else {
    AmpValue = ((Av11 - 2.4) / 0.157) * 10;
    ekran2.setCursor(10, 2);
    ekran2.print("IL:");
    ekran2.setCursor(13, 2);
    ekran2.print("       ");
    ekran2.setCursor(13, 2);
    ekran2.print(AmpValue);
    ekran2.print("mA");
  }

  // Print block, Av11, and AmpValue to serial monitor
  Serial.print("Block= ");
  Serial.println(block);
  Serial.print("Av11= ");
  Serial.println(Av11);
  Serial.print("AmpValue = ");
  Serial.println(AmpValue);

  // Battery Charging Logic
  if (block == 0) {
    if ((AmpValue >= 0.5) && (AmpValue <= 8)) {
      block = 1;
      digitalWrite(RelayPro, HIGH);
      ekran1.setCursor(10, 2);
      ekran1.print("       ");
      ekran1.setCursor(10, 2);
      ekran1.print("FullyChar!");
    }
  }

  if (block == 1) {
    if (AmpValue >= 8.2) {
      block = 0;
      digitalWrite(RelayPro, LOW);
      ekran1.setCursor(10, 2);
      ekran1.print("         ");
      ekran1.setCursor(10, 2);
      ekran1.print("          ");
    }
  }

  // Battery Discharge Logic
  if (block == 0) {
    if ((Cell1 <= DischVolt) || (Cell2 <= DischVolt) || (Cell3 <= DischVolt) || (Cell4 <= DischVolt) || (Cell5 <= DischVolt)) {
      block = 2;
      // Turn off discharge MOSFETs
      digitalWrite(BalMos1, LOW);
      digitalWrite(BalMos2, LOW);
      digitalWrite(BalMos3, LOW);
      digitalWrite(BalMos4, LOW);
      digitalWrite(BalMos5, LOW);
      ekran1.setCursor(10, 2);
      ekran1.print("   ");
      ekran1.setCursor(10, 2);
      ekran1.print("OverDisc!");
    }
  }

  if (block == 2) {
    if ((Cell1 >= DischVolt) && (Cell2 >= DischVolt) && (Cell3 >= DischVolt) && (Cell4 >= DischVolt) && (Cell5 >= DischVolt)) {
      block = 0;
      ekran1.setCursor(10, 2);
      ekran1.print("          ");
      ekran1.setCursor(10, 2);
    }
  }

  // Read temperature from thermistors
  temp1 = 0;
  for (int i = 0; i < 10; i++) {
    Ko = analogRead(Thermistor1Pin);
    R2 = R1 * (1023.0 / (float)Ko - 1.0);
    logR2 = log(R2);
    T1 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    Tc1 = T1 - 273.15;
    temp1 = temp1 + Tc1;
  }
  TF1 = ((temp1) / 10);

  // Similar logic for other thermistors (2 to 5)
  temp2 = 0;
  for(int i=0; i<10; i++)
  {
  Ko = analogRead(Thermistor2Pin);
  R2 = R1 * (1023.0 / (float)Ko - 1.0);
  logR2 = log(R2);
  T2 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc2 = T2 - 273.15;
  temp2 = temp2 + Tc2;
  }
  TF2 = ((temp2)/10);


  temp3=0;
  for(int i=0; i<10; i++)
  {
  Ko = analogRead(Thermistor3Pin);
  R2 = R1 * (1023.0 / (float)Ko - 1.0);
  logR2 = log(R2);
  T3 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc3 = T3 - 273.15;
  temp3 = temp3 + Tc3;
  }
  TF3 = ((temp3)/10);


  temp4=0;
  for(int i=0; i<10; i++)
  { 
  Ko = analogRead(Thermistor4Pin);
  R2 = R1 * (1023.0 / (float)Ko - 1.0);
  logR2 = log(R2);
  T4 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc4 = T4 - 273.15;
  temp4 = temp4 + Tc4;
  }
  TF4 = ((temp4)/10);
  

  temp5=0;
  for(int i=0; i<10; i++)
  { 
  Ko = analogRead(Thermistor5Pin);
  R2 = R1 * (1023.0 / (float)Ko - 1.0);
  logR2 = log(R2);
  T5 = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc5 = T5 - 273.15;
  temp5 = temp5 + Tc5;
  delay(100);
  }
  TF5 = ((temp5)/10);


  // Display OverVoltage on LCD if any cell voltage exceeds protection voltage
  if (lock == 0) {
    if ((Cell1 >= ProtectV) || (Cell2 >= ProtectV) || (Cell3 >= ProtectV) || (Cell4 >= ProtectV) || (Cell5 >= ProtectV)) {
      lock = 1;
      digitalWrite(RelayPro, HIGH);
      ekran2.setCursor(10, 3);
      ekran2.print("   ");
      ekran2.setCursor(10, 3);
      ekran2.print("OverVolt!");
    }
  }

  // Clear OverVoltage condition if all cell voltages are below protection voltage
  if (lock == 1) {
    if ((Cell1 <= ProtectV - 0.05) && (Cell2 <= ProtectV - 0.05) && (Cell3 <= ProtectV - 0.05) && (Cell4 <= ProtectV - 0.05) && (Cell5 <= ProtectV - 0.05)) {
      lock = 0;
      digitalWrite(RelayPro, LOW);
      ekran2.setCursor(10, 3);
      ekran2.print("          ");
      ekran2.setCursor(10, 3);
      ekran2.print("          ");
    }
  }

  // Display OverHeat on LCD if any thermistor temperature exceeds maximum temperature
  if (lock == 0) {
    if ((TF1 >= maxtemp) || (TF2 >= maxtemp) || (TF3 >= maxtemp) || (TF4 >= maxtemp) || (TF5 >= maxtemp)) {
      lock = 2;
      digitalWrite(RelayPro, HIGH);
      ekran2.setCursor(0, 3);
      ekran2.print("   ");
      ekran2.setCursor(0, 3);
      ekran2.print("Overheat!");
    }
  }

  // Clear OverHeat condition if all thermistor temperatures are below maximum temperature
  if (lock == 2) {
    if ((TF1 <= maxtemp) && (TF2 <= maxtemp) && (TF3 <= maxtemp) && (TF4 <= maxtemp) && (TF5 <= maxtemp)) {
      lock = 0;
      digitalWrite(RelayPro, LOW);
      ekran2.setCursor(0, 3);
      ekran2.print("   ");
      ekran2.setCursor(0, 3);
      ekran2.print("             ");
    }
  }
}


  // Initialize sum variables for Cell1 to Cell10
Sum1 = 0;
Sum2 = 0;
Sum3 = 0;
Sum4 = 0;
Sum5 = 0;
Sum6 = 0;
Sum7 = 0;
Sum8 = 0;
Sum9 = 0;
Sum10 = 0;

// Loop to read analog values from A0, calculate voltage, and accumulate sum for Cell1 (Av1)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A0);
  V1 = ((analogvalue * 5.79) / 1230);
  Vs1 = V1 / (r2 / (r1 + r2));
  Sum1 = Sum1 + Vs1;
}
Av1 = (Sum1 / 45);

// Similar loop for Cell2 (Av2)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A1);
  V2 = ((analogvalue * 5.80) / 1230);
  Vs2 = V2 / (r2 / (r1 + r2));
  Sum2 = Sum2 + Vs2;
}
Av2 = (Sum2 / 45);

// Similar loop for Cell3 (Av3)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A2);
  V3 = ((analogvalue * 5.83) / 1230);
  Vs3 = V3 / (r2 / (r1 + r2));
  Sum3 = Sum3 + Vs3;
}
Av3 = (Sum3 / 45);

// Similar loop for Cell4 (Av4)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A3);
  V4 = ((analogvalue * 5.80) / 1230);
  Vs4 = V4 / (r2 / (r1 + r2));
  Sum4 = Sum4 + Vs4;
}
Av4 = (Sum4 / 45);

// Similar loop for Cell5 (Av5)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A4);
  V5 = ((analogvalue * 5.80) / 1230);
  Vs5 = V5 / (r2 / (r1 + r2));
  Sum5 = Sum5 + Vs5;
}
Av5 = (Sum5 / 45);

// Similar loop for Cell6 (Av6)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A5);
  V6 = ((analogvalue * 5.80) / 1230);
  Vs6 = V6 / (r2 / (r1 + r2));
  Sum6 = Sum6 + Vs6;
}
Av6 = (Sum6 / 45);

// Similar loop for Cell7 (Av7)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A6);
  V7 = ((analogvalue * 5.82) / 1230);
  Vs7 = V7 / (r2 / (r1 + r2));
  Sum7 = Sum7 + Vs7;
}
Av7 = (Sum7 / 45);

// Similar loop for Cell8 (Av8)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A7);
  V8 = ((analogvalue * 5.89) / 1230);
  Vs8 = V8 / (r2 / (r1 + r2));
  Sum8 = Sum8 + Vs8;
}
Av8 = (Sum8 / 45);

// Similar loop for Cell9 (Av9)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A8);
  V9 = ((analogvalue * 5.90) / 1230);
  Vs9 = V9 / (r2 / (r1 + r2));
  Sum9 = Sum9 + Vs9;
}
Av9 = (Sum9 / 45);

// Similar loop for Cell10 (Av10)
for (int i = 0; i < 45; i++) {
  int analogvalue = analogRead(A9);
  V10 = ((analogvalue * 6.28) / 1230);
  Vs10 = V10 / (r2 / (r1 + r2));
  Sum10 = Sum10 + Vs10;
}
Av10 = (Sum10 / 45);

// Calculate individual cell voltages (Cell1 to Cell5)
Cell1 = Av1 - Av2;
Cell2 = Av4 - Av3;
Cell3 = Av5 - Av6;
Cell4 = Av7 - Av8;
Cell5 = Av9 - Av10;

  
  
 // Find minimum cell voltage (Vmin) and differences between each cell voltage and Vmin
if ((Cell1 <= Cell2) && (Cell1 <= Cell3) && (Cell1 <= Cell4) && (Cell1 <= Cell5))
{
  Vmin = Cell1;
  dif1 = Cell2 - Cell1;     
  dif2 = Cell3 - Cell1;
  dif3 = Cell4 - Cell1;
  dif4 = Cell5 - Cell1;
}
else if ((Cell2 <= Cell1) && (Cell2 <= Cell3) && (Cell2 <= Cell4) && (Cell2 <= Cell5))
{
  Vmin = Cell2;
  dif1 = Cell1 - Cell2;
  dif2 = Cell3 - Cell2;
  dif3 = Cell4 - Cell2;
  dif4 = Cell5 - Cell2;
}
else if ((Cell3 <= Cell1) && (Cell3 <= Cell2) && (Cell3 <= Cell4) && (Cell3 <= Cell5))
{
  Vmin = Cell3;
  dif1 = Cell1 - Cell3;
  dif2 = Cell2 - Cell3;
  dif3 = Cell4 - Cell3;
  dif4 = Cell5 - Cell3;
}
else if ((Cell4 <= Cell1) && (Cell4 <= Cell2) && (Cell4 <= Cell3) && (Cell4 <= Cell5))
{
  Vmin = Cell4;
  dif1 = Cell1 - Cell4;
  dif2 = Cell2 - Cell4;
  dif3 = Cell3 - Cell4;
  dif4 = Cell5 - Cell4;
}
else
{
  Vmin = Cell5;
  dif1 = Cell1 - Cell5;
  dif2 = Cell2 - Cell5;
  dif3 = Cell3 - Cell5;
  dif4 = Cell4 - Cell5;
}

// Balance cells based on BalanceVal
if (Cell1 >= BalanceVal){
  bal1 = 1;
  digitalWrite(BalMos1, HIGH); // Activate Discharge MOSFET1
  ekran1.setCursor(0, 3);
  ekran1.print("   ");
  ekran1.setCursor(0, 3);
  ekran1.print("Dschr:");
  ekran1.setCursor(6, 3);
  ekran1.print("B1");
}

if (Cell2 >= BalanceVal){
  bal2 = 1;
  digitalWrite(BalMos2, HIGH); // Activate Discharge MOSFET2
  ekran1.setCursor(9, 3);
  ekran1.print("B2");
}

if (Cell3 >= BalanceVal){
  bal3 = 1;
  digitalWrite(BalMos3, HIGH); // Activate Discharge MOSFET3
  ekran1.setCursor(12, 3);
  ekran1.print("B3");
}

if (Cell4 >= BalanceVal){
  bal4 = 1;
  digitalWrite(BalMos4, HIGH); // Activate Discharge MOSFET4
  ekran1.setCursor(15, 3);
  ekran1.print("B4");
}

if (Cell5 >= BalanceVal){
  bal5 = 1;
  digitalWrite(BalMos5, HIGH); // Activate Discharge MOSFET5
  ekran1.setCursor(18, 3);
  ekran1.print("B5");
}

// Balance cells if they are above Vmin
if (bal1 == 1){
  if (Cell1 <= Vmin){
    bal1 = 0;
    digitalWrite(BalMos1, LOW); // Turn off Discharge MOSFET1
    ekran1.setCursor(0, 3);
    ekran1.print("      ");
    ekran1.setCursor(0, 3);
    ekran1.setCursor(6, 3);
    ekran1.print("   ");
  }
}

if (bal2 == 1){
  if (Cell2 <= Vmin){
    bal2 = 0;
    digitalWrite(BalMos2, LOW); // Turn off Discharge MOSFET2
    ekran1.setCursor(0, 3);
    ekran1.print("      ");
    ekran1.setCursor(0, 3);
    ekran1.setCursor(9, 3);
    ekran1.print("   ");
  }
}

if (bal3 == 1){
  if (Cell3 <= Vmin){
    bal3 = 0;
    digitalWrite(BalMos3, LOW); // Turn off Discharge MOSFET3
    ekran1.setCursor(0, 3);
    ekran1.print("        ");
    ekran1.setCursor(0, 3);
    ekran1.setCursor(12, 3);
    ekran1.print("   ");
  }
}

if (bal4 == 1){
  if (Cell4 <= Vmin){
    bal4 = 0;
    digitalWrite(BalMos4, LOW); // Turn off Discharge MOSFET4
    ekran1.setCursor(0, 3);
    ekran1.print("        ");
    ekran1.setCursor(0, 3);
    ekran1.setCursor(15 , 3);
    ekran1.print("   ");
  }
}

if (bal5 == 1){
  if (Cell5 <= Vmin){
    bal5 = 0;
    digitalWrite(BalMos5, LOW); // Turn off Discharge MOSFET5
    ekran1.setCursor(0, 3);
    ekran1.print("        ");
    ekran1.setCursor(0, 3);
    ekran1.setCursor(18, 3);
    ekran1.print("   ");
  }
}

// Display balancing information on the screen
if ((bal1 == 1) || (bal2 == 1) || (bal3 == 1) || (bal4 == 1) || (bal5 == 1)){
  ekran1.setCursor(0, 3);
  ekran1.print("   ");
  ekran1.setCursor(0, 3);
  ekran1.print("Dschr:");
}
else {
  ekran1.setCursor(0, 3);
  ekran1.print("        ");
  ekran1.setCursor(0, 3);
}


// Display cell voltages on ekran1
ekran1.setCursor(0, 0);
ekran1.print("B1:");
ekran1.setCursor(3, 0);
ekran1.print("       ");
ekran1.setCursor(3, 0);
ekran1.print(Cell1);
ekran1.print("V");

ekran1.setCursor(10, 0);
ekran1.print("B2:");
ekran1.setCursor(13, 0);
ekran1.print("       ");
ekran1.setCursor(13, 0);
ekran1.print(Cell2);
ekran1.print("V");

ekran1.setCursor(0, 1);
ekran1.print("B3:");
ekran1.setCursor(3, 1);
ekran1.print("       ");
ekran1.setCursor(3, 1);
ekran1.print(Cell3);
ekran1.print("V");

ekran1.setCursor(10, 1);
ekran1.print("B4:");
ekran1.setCursor(13, 1);
ekran1.print("       ");
ekran1.setCursor(13, 1);
ekran1.print(Cell4);
ekran1.print("V");

ekran1.setCursor(0, 2);
ekran1.print("B5:");
ekran1.setCursor(3, 2);
ekran1.print("       ");
ekran1.setCursor(3, 2);
ekran1.print(Cell5);
ekran1.print("V");

// Display temperature readings on ekran2
ekran2.setCursor(0, 0);
ekran2.print("T1:");
ekran2.setCursor(3, 0);
ekran2.print("       ");
ekran2.setCursor(3, 0);
ekran2.print(TF1);
ekran2.print("C");

ekran2.setCursor(10, 0);
ekran2.print("T2:");
ekran2.setCursor(13, 0);
ekran2.print("       ");
ekran2.setCursor(13, 0);
ekran2.print(TF2);
ekran2.print("C");

ekran2.setCursor(0, 1);
ekran2.print("T3:");
ekran2.setCursor(3, 1);
ekran2.print("       ");
ekran2.setCursor(3, 1);
ekran2.print(TF3);
ekran2.print("C");

ekran2.setCursor(10, 1);
ekran2.print("T4:");
ekran2.setCursor(13, 1);
ekran2.print("       ");
ekran2.setCursor(13, 1);
ekran2.print(TF4);
ekran2.print("C");

ekran2.setCursor(0, 2);
ekran2.print("T5:");
ekran2.setCursor(3, 2);
ekran2.print("    ");
ekran2.setCursor(3, 2);
ekran2.print(TF5);
ekran2.print("C");
