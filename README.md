# 2.4GHz NRF Messenger

A portable messaging device using NRF24L01+ modules for communication on the 2.4GHz band with an effective range of 1-2km.

<img width="683" height="312" alt="image" src="https://github.com/user-attachments/assets/e4840aff-57b4-4186-8ff4-360dc6e29fc6" />

<img width="600" height="325" alt="image" src="https://github.com/user-attachments/assets/97e54c60-4e8f-46b6-a025-fdda1a7a645b" />

## Features
- Wireless text messaging over 2.4GHz band
- 1-2km effective range (line-of-sight)
- OLED display for message viewing
- 5-button interface for message input
- Li-Ion battery powered

## Hardware Components
- Arduino Nano (ATmega328P)
- NRF24L01 2.4GHz transceiver module
- SSD1306 128x64 OLED display
- AMS1117-3.3 voltage regulator
- TP4056 Li-Ion battery charger
- 3.7V Li-Ion battery
- Tactile buttons (x5)
- Power switch

## Wiring Diagram

### NRF24L01+ Connections
| NRF24L01+ | Arduino Nano | AMS1117-3.3 |
|-----------|--------------|-------------|
| VCC       |              | OUT         |
| GND       | GND          | GND         |
| CE        | D8           |             |
| CSN       | D7           |             |
| SCK       | D13          |             |
| MOSI      | D11          |             |
| MISO      | D12          |             |
|           | +5V          | IN          |

### SSD1306 OLED Connections
| SSD1306 | Arduino Nano |
|---------|--------------|
| VCC     | +5V          |
| GND     | GND          |
| SDA     | A4           |
| SCL     | A5           |

### Button Connections
| Button | Arduino Nano |
|--------|--------------|
| UP     | D2           |
| DOWN   | D3           |
| LEFT   | D4           |
| RIGHT  | D5           |
| MODE   | D9           |

### Power System
Li-Ion 3.7V Battery → TP4056 (B+/B-)
<br/>
TP4056 OUT → Switch → Arduino Nano VCC/GND

## Power Management
- 3.7V Li-Ion battery (18650 or similar)
- TP4056 charging module for safe battery charging
- AMS1117-3.3 regulator for NRF24L01+ module
- Power switch for complete power control

## Software Requirements
- Arduino IDE
- RF24 library for NRF24L01
- Adafruit SSD1306 library for OLED
- Adafruit GFX library

## Installation
1. Connect all components according to the wiring diagram
2. Install required Arduino libraries
3. Upload the sketch to Arduino Nano
4. Power the device with 3.7V Li-Ion battery

## Usage
1. Power on the device using the switch
2. Use the buttons to navigate 
3. Compose messages using the on-screen keyboard
4. Send/receive messages with same devices

## Range Considerations
For best results (1-2km range):
- Use in open areas with line-of-sight
- Consider adding external antennas to NRF24L01+ modules
- Higher elevations improve range
- Avoid 2.4GHz interference sources

## Feedback

Feel free to create a pull request or issue!
