# Terminal-Style Cellular Device

A DIY, terminal-first cellular communication device designed as a **personal network terminal** rather than a modern smartphone.

This project intentionally delays committing to a specific cellular modem and provider in order to keep the architecture modular, inspectable, and adaptable as constraints evolve.

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

---

## Design Philosophy

### Constraints as a Design Tool

Instead of fighting limitations, the project embraces them:

* Text-only interface
* No app layer
* No multitasking
* Single firmware image

Every feature must justify its existence.

---

## Hardware Overview (Abstracted)

| Subsystem       | Description                                  |
| --------------- | -------------------------------------------- |
| Microcontroller | Main command processor and UI controller     |
| Cellular Modem  | LTE / cellular interface (provider-agnostic) |
| Display         | Monochrome terminal output                   |
| Input           | Keyboard matrix + rotary encoder             |
| Power           | Li-ion battery + regulation                  |
| Antennas        | Cellular + optional GNSS                     |

Specific modem and provider choices are intentionally deferred to keep the project flexible during early development.

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

## Firmware Structure (Planned)

```
firmware/
├─ core/
│  ├─ input_manager/
│  ├─ display_driver/
│  ├─ modem_interface/
│  ├─ command_parser/
│  ├─ power_manager/
│  └─ logger/
└─ main.c
```

---

## Command Interface (Planned)

```
sms send <number> <message>
sms read
gps get
status
net info
log show
```

Incoming messages may also be parsed as structured commands, allowing limited remote interaction.

---

## Modem & Provider Selection (Deferred)

The choice of cellular modem and network provider is intentionally postponed.

This allows:

* Evaluation of power consumption vs. capability
* Comparison of LTE Cat-M, NB-IoT, or other options
* Flexibility in data plans and coverage

The architecture assumes a modem controlled via a command interface (e.g., AT commands), but avoids hard dependencies until later development stages.

---

## Development Status

* [x] Project scope defined
* [x] System architecture outlined
* [ ] Modem selection
* [ ] Power subsystem validation
* [ ] Basic terminal UI
* [ ] Input handling
* [ ] Cellular communication
* [ ] Optimization and refinement

---

## What This Is (and Isn’t)

✅ A personal communication terminal
✅ A learning platform for embedded and networking systems
✅ A constraint-driven engineering project

❌ A smartphone
❌ An app platform
❌ A consumer product

---

## License

MIT (subject to change)
