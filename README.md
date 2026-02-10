# Terminal-Style Cellular Device

A DIY, terminal-first cellular communication device designed as a **personal network terminal** rather than a modern smartphone.

**Take a look at the [Hackaday Project Page](https://hackaday.io/project/204892-a-terminal-style-phone-for-the-geek) for more information!**

---

## Why This Exists

Modern phones abstract communication behind apps, notifications, and opaque systems that prioritize engagement over understanding.

This project treats cellular connectivity as what it fundamentally is:

> **A network interface.**

No apps.
No touch screen.
No background processes.

Just commands, responses, and visibility into the system.

---

## Core Features (Target)

* ✉️ Send and receive SMS with standard phones
* ⌨️ Physical keyboard for direct command entry
* 🖥️ Monochrome display with terminal-style UI
* 🔁 Rotary encoder for scrolling and command history
* 🔋 Battery-powered with aggressive power management
* 🧭 Optional GPS/location reporting

All features are implemented explicitly and synchronously; nothing happens in the background without user intent.

## What This Is (and Isn’t)

✅ A personal communication terminal

✅ A learning platform for embedded and networking systems

✅ A constraint-driven engineering project

❌ A smartphone

❌ An app platform

❌ A consumer product

---

## Design Philosophy

### What makes this GEEKY

* Text-only interface
* No app layer
* No multitasking
* Clear case and beautiful PCB

---

## Hardware Overview (Abstracted)

| Subsystem       | Description                                  |
| --------------- | -------------------------------------------- |
| Microcontroller | Main command processor and UI controller     |
| Cellular Modem  | LTE-M / cellular interface                     |
| Display         | Monochrome terminal output                   |
| Input           | Keyboard matrix + rotary encoder             |
| Power           | Li-ion battery + regulation                  |
| Antennas        | Cellular + optional GNSS                     |

---

## System Architecture

* The microcontroller acts as the **command processor**
* The cellular modem is treated as a **peripheral**, controlled via an explicit command interface
* UI is rendered as a scrolling terminal
* All communication actions are user-initiated

Example interaction:

```
> status
NET: CONNECTED
RSSI: -89 dBm
BAT: 82%
```

---

---

## Command Interface (Planned)

```
sms send <contact> <message>
sms read
gps get
status
net info
log show
```

Incoming messages may also be parsed as structured commands, allowing limited remote interaction.

---

---

## Development Status

* [x] Project scope defined
* [x] System architecture outlined
* [x] Modem selection
* [ ] Power subsystem validation
* [ ] Basic terminal UI
* [ ] Input handling
* [ ] Cellular communication
* [ ] Optimization and refinement

---

## Dev Log

### January 20, 2026
* Ordered SIM7080G as modem choice
* Ordered a SIM card from Hologram (Thanks a lot to Felix from Hologram for all of your help!)

### January 27, 2026
* SIM7080G modem module came in from AliExpress

### February 1, 2026
Added a complete web-to-Arduino chat system using a SIM7080G modem, including the Arduino sketch, a Flask-based backend, and a simple web frontend. The changes enable bidirectional messaging between a web interface and an Arduino device via HTTP endpoints, using a SQLite database for message storage and retrieval.

### February 9, 2026
Implemented hardware prototype of test terminal interface with Arduino UNO and TFT screen. Text input is through serial port. Link to more details on the [Hackaday Post](https://hackaday.io/project/204892-a-terminal-style-phone-for-the-geek/log/246067-terminal-on-an-arduino-with-tft-display).

## License

MIT (subject to change)
