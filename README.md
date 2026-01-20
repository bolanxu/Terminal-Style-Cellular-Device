# Terminal-Style Cellular Device

A DIY, terminal-first cellular communication device designed as a **personal network terminal** rather than a modern smartphone.

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

## License

MIT (subject to change)
