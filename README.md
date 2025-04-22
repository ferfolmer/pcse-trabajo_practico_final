# Monitor Ambiental – Nucleo‑F429ZI + BME280

## Descripción general
Pequeño **monitor ambiental** que mide temperatura, presión y humedad con un sensor **BME280** mediante protocolo **I2C**.  
Los valores se presentan en un **LCD 16×2** y el usuario navega con un **encoder rotativo HW‑040** para:
* recorrer un menú de opciones,  
* aplicar **offsets de calibración** a cada variable,  
* restablecer la calibración a cero.  

El sistema registra cada acción por **UART 115200 bps** para depuración.

---

## Plataforma de hardware
| Componente           | Rol               | Interfaz     |
|----------------------|-------------------|-------------|
| STM32 Nucleo‑F429ZI | MCU, coordinador  | –           |
| Módulo BME280        | Sensor T / H / P  | I²C @ 100 kHz |
| LCD 16×2 + PCF8574   | Salida local      | I²C @ 100 kHz |
| Encoder HW‑040       | Input de usuario  | GPIO + Pull‑ups |
| PC + ST-Link         | Programación y alimentación 5 V | SWD, USB‑VCP |

---

## Características principales
* **Dos protocolos**: I²C (sensor + LCD) y UART (comunicación PC).  
* **Arquitectura modular**:  
  `/Drivers/API` (alto nivel) · `/Drivers/Port` (HW específico).  
* Diseño **sin bloqueo**; retardos con temporizador `HAL_GetTick()`.  
* Registro de estado y errores por UART.

---

## Objetivo de los módulos de la API

### API I²C
Proporciona la capa de comunicación **I²C** unificada para el sensor BME280 y el LCD. Gestiona la inicialización del bus, envíos y recepciones de datos, garantizando que ambos periféricos intercambien información de manera transparente.

### API UART
Ofrece una interfaz de **depuración** y registro de eventos por **UART**. Permite enviar mensajes de estado, errores y confirmaciones al PC, facilitando el diagnóstico y seguimiento de la operación en tiempo real.

### API LCD
Encapsula el manejo de la pantalla **HD44780** mediante el expansor PCF8574. Simplifica la impresión de texto, limpieza de pantalla y posicionamiento de cursor, centralizando la lógica de display para el menú y las lecturas del sensor.

### API BME280
Interfaz dedicada al sensor **BME280**, responsable de inicializar el dispositivo, leer datos brutos, aplicar compensación y gestionar calibración con offsets. Entrega al nivel superior valores de temperatura, presión y humedad listos para mostrar o usar en lógica de control.

---

## Estructura del repositorio
```
├─ Core/                 código CubeIDE
├─ Drivers/
│   ├─ API/              drivers genéricos (inc / src)
│   └─ Port/             acceso HW (inc / src)
├─ docs/                 diagramas e imágenes
└─ README.md
```
Cada driver genérico expone solo sus funciones públicas en `/inc` y separa la lógica de hardware en `/src` con porting en `Drivers/Port`.  

---
## Diagrama de Máquina de estados finita
Se incluye a continuación la MEF que maneja cómo se muestra el menú en el LCD y detalles de la navegación en el mismo.
![alt](/docs/images/mef_menu.png) 

---

## Uso
* **Pantalla principal**: T / H / P en tiempo real (1 s).  
* **Click** del encoder → entra/confirma.  
* **Giro CW / CCW** → avanza/retrocede en el menú.  
* Submenús *Offset*: girar para ajustar, click para guardar.  
* *Reset Calibración* pone todos los offsets en 0.  
* Cada evento se informa por UART.

---

## Próximos pasos
* Migrar UART e I²C a DMA/IT.  

---

## Licencia
Proyecto bajo licencia **MIT**.  
© 2025 Fernando Folmer — Carrera de Especialización en Sistemas Embebidos.
