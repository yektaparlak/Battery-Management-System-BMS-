# Battery-Management-System-BMS-
Passive Cell Battery Management System (BMS) Balancing Design for a LiFePO4 Battery Pack

# Battery Management System (BMS) for LiFePO4 Battery Pack
<img width="310" alt="image" src="https://github.com/yektaparlak/Battery-Management-System-BMS-/assets/111290340/71071b84-aae5-46c9-88b4-22c09621e222"> <samp>***(logo design by Yekta Parlak)***</samp>

<br> 

## 1. Header Inclusions and Variable Definitions:

- **Libraries:**
  - Includes necessary libraries for screen control (LiquidCrystal_I2C).

- **Variables:**
  - Voltage readings (Vs1 to Vs11, V1 to V11, Sum1 to Sum11, Av1 to Av11)
  - Calculated cell voltages (Cell1 to Cell5)
  - Current sense (AmpValue)
  - Thermistor readings (TF1 to TF5)
  - Temperatures (temp1 to temp5)
  - Pin assignments (RelayPro, BalMos1 to BalMos5, Thermistor1Pin to Thermistor5Pin, A0 to A15)
  - State variables (block, lock) initialized to 0:
    - `block`: Represents a flag or state variable, possibly indicating a charging or discharging state. The initial value of 0 suggests that the system is not in a blocked state at the beginning.
    - `lock`: This variable's purpose is not immediately clear from the provided code snippet. It could be used as a lock or synchronization mechanism to control access to certain sections of the code or resources. The initial value of 0 might mean that the lock is not engaged or is available.

## 2. `setup()` Function:

- Initializes serial communication, LCD screens, and pin modes for inputs and outputs.

## 3. `loop()` Function:

### Current Sensing:

- Reads current value (Av11) from analog input A15.
- Calculates current in mA (AmpValue).
- Displays current on LCD screen.

### Charging and Discharging Control:

- Monitors current (AmpValue) and cell voltages (Cell1 to Cell5).
- Manages charging and discharging states (block variable) based on current and voltage thresholds.

### Overvoltage and Overheat Protection:

- Monitors cell voltages (Cell1 to Cell5) and temperatures (TF1 to TF5).
- Activates protection relay (RelayPro) and displays warnings on LCD screens if overvoltage or overheat conditions occur.

### Cell Voltage Readings:

- Takes multiple voltage readings from analog inputs (A0 to A9).
- Averages readings for each cell (Av1 to Av5).
- Calculates cell voltages (Cell1 to Cell5).

### Balancing Control:

- Compares cell voltages to a reference voltage (BalanceVal).
- Activates MOSFETs (BalMos1 to BalMos5) to discharge cells that are higher than the reference.

### Temperature Readings:

- Reads thermistor values from analog inputs (A10 to A14).
- Calculates temperatures (TF1 to TF5) using Steinhart-Hart equation.

### LCD Display:

- Displays cell voltages, temperatures, current, and warnings on the two LCD screens.

## 4. Key Points:

- The code manages a battery pack with 5 cells, incorporating current sensing, balancing, and protection features.
- It utilizes multiple analog inputs for voltage and temperature readings.
- MOSFETs control cell balancing, and a relay is used for protection.
- Information and warnings are displayed on two LCD screens.



# Flowchart of Overall Design:
<img width="4000" alt="image" src="https://github.com/yektaparlak/Battery-Management-System-BMS-/assets/111290340/09de0c9c-09f4-48d7-94a2-8be07dfa6dc5">

# The Temperature Control Unit:
![image](https://github.com/yektaparlak/Battery-Management-System-BMS-/assets/111290340/d702f71c-f2a3-4473-9a57-59a644a968b1)


# The Voltage Measurement Unit:
![image](https://github.com/yektaparlak/Battery-Management-System-BMS-/assets/111290340/0788fe17-b6d4-4f9e-9c4a-7570e883f2f8)

