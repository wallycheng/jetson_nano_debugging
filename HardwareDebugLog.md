Hrdware Debug Log: Motor Driver & Power Supply Issues

## Issue 1: SY8205 Voltage Drop
- **Description**:  
  Four 1.3V batteries in series output 5.25V, but the voltage drops to 4.7V after passing through the SY8205 buck converter.
- **Hypothesis**:  
  Potential manufacturing defect in the SY8205 module.
- **Action**:  
  Proceeded with testing despite risks to the PCB (voltage instability might damage components).

---

## Issue 2: Jetson Nano GPIO & L7110S Motor Driver
- **Setup**:  
  - Jetson Nano GPIO pin 27 outputs 3V to L7110S motor driver (B1/B2 connected to GND).
  - **Expected**: MOTORB should drive wheels.
  - **Observed**: MOTORB outputs no power.

- **Voltage Measurements**

| Nodes       | Voltage (V) | Notes                        |
|-------------|-------------|------------------------------|
| A - B       | 2.9         | Within tolerance             |
| OA - OB     | 0           | Critical issue               |
| VCC - OA    | 5           | Power supply functional      |
| OA - GND    | 6           | Too high (DS: <0.25V)        |
| OB - GND    | 3           | Exceeds spec (DS: <0.25V)    |

- **Debug Steps**
1. **Power Supply Adjustment**:
   - Switched Jetson Nano’s power supply to match the PCB’s source (SY8205).
   - **Result**: Motor spun briefly (~1s), then Nano shut down. Repeated attempts yielded identical behavior.

2. **Root Cause Analysis**:
   - SY8205 inefficiency likely triggered U-Boot’s protection mechanism due to unstable voltage.
   - **Solution**: Connected both power bank and battery GNDs. System stabilized, motor functioned normally.

---

## Issue 3: Network Connectivity Issue
- **Description**:
  The device is inaccessible via network, and the `ping` command returns "Host is down."
- **Hypothesis**:
  The network interface may be temporarily unavailable due to electrostatic discharge (ESD) or power-related issues.
- **Action**:
  Attempted to ground the power pin to release ESD. After waiting for 2 minutes, the network connection was restored.

## Issue 4: GPIO Control Failure
- **Description**:
  The control program failed to run. GDB analysis revealed that `gpiod_chip_open /dev/gpiochip0` failed, returning `0x0`.
- **Hypothesis**:
  The issue might be caused by incorrect file permissions for `/dev/gpiochip0`.
- **Action**:
  Checked the permissions of `/dev/gpiochip0` and found it was set to `crw-------` (only accessible by root). Used `sudo chmod 666 /dev/gpiochip0` to modify the permissions, after which the program ran successfully.

## Issue 5: L9110s Voltage Anomaly
- **Description**:
  Abnormal voltage observed on the input pins of the L9110s motor driver module:

| +  | -   | voltage |
|----|-----|---------|
| A1 | GND | 4.4 V   |
| A2 | GND | 4.4 V   |

- **Hypothesis**:
The input pins are connected to VCC through pull-up resistors, setting them to a "high" logic state by default. This keeps the motor driver module in a "brake" state until a "low" signal is applied to drive the motors.
- **Action**:
Verified the circuit design to ensure the input signals can correctly control the motor driver module's state.

## Issue 5: Gpio-pins didn't work

- **Description**
  Fail to set  status to gpio-pin "10 13 19 26 27" on the carrier board with **gpiod**. The pin voltage measurement data didn't match expections.

  Steps to Reproduce:

```cpp
const int aim_pin = 10; // trying to twiddle pin 10 on the carrier board
gpio_line* aim_line = gpiod_chip_get_line(opened_chip, aim_pin); // trying to get line by pin name, what is wrong  
```

- **Hypothesis**:
  Not sure if there are any perrequisite steps required before using the GPIO pins on the Jetson Nano, or if the carrier board pin numbers do not match the CPU-side line numbers.

- **Debug Steps**:
1. gpioinfo > tmp0

2. run ctrl

3. gpioinfo > tmp1

4. diff tmp0 tmp1

5. As shown in the printed output, I triggered the wrong line actually:
| line 10 after.  | before |
|-----------------|--------|
| line  10:      unnamed       unused  output  active-high   | line  10:      unnamed       unused   input  active-high  |
  There is a [chat](https://jetsonhacks.com/nvidia-jetson-nano-j41-header-pinout/) shown gpio pinout on the borad.
