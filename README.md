# ContadorSiembra
Sistema para contabilizar pulsos y accionar una salida digital de uso específico

# Especificación de Hardware

Todas las entradas/salidas digitales son con lógica de 3.3V

- MCU: Atmega 328 PU (16Mhz)
- DISPLAY: LCD 16x02 + PCF8574
- PULSADORES: 3 (Menu, Subir, Bajar)
- SENSOR: Accionamiento digital de tipo colector abierto (Transistor u Opto acoplador)
- SALIDA: Pulso digital de 500ms (Relé u Opto acoplador)

# Pinout de ATMEGA328
| PIN | CONFIGURACION | FUNCION |
|-----|---------------|---------|
| D02 | INPUT + PULLUP | Entrada de pulsos |
| D03 | INPUT + PULLUP | Menú de configuración |
| D08 | INPUT + PULLUP | Menú: aumentar valor de parámetro, Reinicio: Reset de fábrica |
| D09 | INPUT PULLUP | Menú: disminuír valor de parámetro, Reinicio: Reset de fábrica |
| D10 | OUTPUT | Salida a relé |
| A04 | I2C SDA + PULLUP | Display 16x2 + EEPROM |
| A05 | I2C SCL + PULLUP | Display 16x2 + EEPROM |

# Descripción de funcionamiento:
El sistema cuenta los pulsos de desde la entrada digital **[D02]** y compara con el valor dado por la variable **Divisor** (Ver menú de configuración). Cuando el contador de pulsos es igual al valor **Divisor** genera un pulso de salida en **[D10]** para accionar un relé, diodo o transistor.

## Pantalla principal

- **ACCION**: Debajo se muestra el contador accionamientos (pulsos de salida generados)
- **C** (Contador): A la derecha se muestra el contador pulsos de entrada.
- **D** (Divisor): A la derecha se muestra el valor **Divisor** almacenado en la configuración.

```
ACCION  | C: XXXX
XXXXXXXX| L: XXXX
```

## Menú de configuración
- Presionando el pulsador de **MENU** se ingresa a la pantalla de configuración.
- Una vez dentro de este modo, se habilitan los botones **ARRIBA** y **ABAJO** que modifican el parámetro **Divisor**.
- Se sale del menú aceptando los cambios presionando el pulsador **MENU** nuevamente.

```
CONFIGURACION
> Divisor:  XXXX
```

## Reseteo de fábrica
- Apagr el equipo
- Mantener presionado los pulsadores **ARRIBA** y **ABAJO** a la vez.
- Encender el equipo sin soltar los pulsadores y esperar el mesaje "FACTORY RESET"
- Soltar los pulsadores y operar normalmente. El parámetro **Divisor** volverá a su valor original "1".
  
```
FACTORY RESET

```

