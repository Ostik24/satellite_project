# Satellite prototype

CubeSats, miniature satellites of a standard format, are increasingly used for scientific research in space. 
This repository  presents the development of a CubeSat prototype designed to investigate the growth of mold 
under microgravity and increased cosmic radiation. The primary objective of this experiment is to explore 
how extreme space conditions affect the life cycle and development of microorganisms, providing valuable 
insights for future space missions and astronaut safety.

### Features
- **Power Management System**: Code and algorithms for efficient power distribution using solar panels, batteries, and voltage regulators.
- **Thermometer Integration**: Implementation of a communication interface for the DS18B20 temperature sensor, enabling onboard temperature monitoring.

### Prerequisites
- **Software**: 
  - STM32CubeIDE.
  - Arduino IDE (testing with Arduino Nano).
  - Required libraries: OneWire (for DS18B20 integration).
- **Onboard Computer**
  - STM32-F411E-DISCO
- **Engineering Instruments**
  - Thermometer
- **Power System Components**
  - DC-DC Step-Down Converter
  - Arduino Nano
  - 4-Channel 12V Relay Module
  - Schottky Diodes
  - 4 Batteries in 2 Holders
  - 3 Solar Panels
  - ACS712 Current Sensor
  - LM2596 Step-Down Converter
  - Terminal Blocks
  - Charge Controller with TP5100 Chip
  - Voltage Dividers with Resistors and Capacitors

### Setting Up the Power System
Connect the batteries, solar panels, and voltage regulators according to the schematic:
![CubeSat Power System](Schematic_Simple-Power-managment.pdf)

## Acknowledgments
- **Team Members**: Sofiia Sampara, Sofiia Popeniuk, Ostap Pavlyshyn
- **Mentor**: Andriy Moroz
