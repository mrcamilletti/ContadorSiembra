# ContadorSiembra
Sistema para contabilizar pulsos y accionar una salida digital de uso específico

# Especificación de Hardware

Todas las entradas/salidas digitales son con lógica de 3.3V

- MCU: Atmega 328 PU (16Mhz)
- EEPROM: AT24C32N
- DISPLAY: LCD 16x02 + PCF8574
- PULSADORES: 3 (Menu, Subir, Bajar)
- SENSOR: Accionamiento digital de tipo colector abierto (Transistor u Opto acoplador)
- SALIDA: Pulso digital de 500ms (Relé u Opto acoplador)

# Pinout de ATMEGA328
| PIN | CONFIGURACION | FUNCION |
|-----|---------------|---------|
| D02 | INPUT + PULLUP | Entrada de pulsos |
| D03 | INPUT + PULLUP | Menú de configuración |
| D08 | INPUT + PULLUP | Menú: aumentar valor de parámetro |
| D09 | INPUT PULLUP | Menú: disminuír valor de parámetro |
| D10 | OUTPUT | Salida a relé |
| A04 | I2C SDA + PULLUP | Display 16x2 + EEPROM |
| A05 | I2C SCL + PULLUP | Display 16x2 + EEPROM |
