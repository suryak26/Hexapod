# Hexapod Robot Project

A six-legged walking robot combining an STM32 + Zephyr RTOS real-time controller with a 
Raspberry Pi 4 co-processor for AI-assisted autonomous navigation. This repository covers 
the full system: firmware, ROS2 integration, custom PCB design, and simulation.

---

## 🆕 Latest Advancements
- **Custom PCB** — replacing the earlier breadboard/module-based wiring with a dedicated 
  purpose-built board for power distribution, servo control, and sensor interfacing
- **ROS simulation** — the robot's kinematics and gait behavior have been taken to 
  simulation level (Gazebo), allowing testing and tuning before deployment on hardware

---

## 🧭 Why a Hexapod?
A hexapod always keeps at least three legs on the ground while walking, making it inherently 
stable — the same principle that lets ants and beetles walk confidently on uneven terrain. 
Six legs also enable a tripod gait (three legs swing forward while three push the body), 
the fastest stable walking pattern, and the robot can still walk on five legs if one fails.

Each leg has 3 joints — **Coxa** (hip, rotates left/right), **Femur** (thigh, lifts/lowers), 
and **Tibia** (shin, extends/retracts) — for a total of **18 servo motors** across 6 legs.

---

## 🏗️ System Architecture

| STM32 + Zephyr RTOS (Lead) | Raspberry Pi 4 (Co-Processor) |
|---|---|
| Controls all 18 servo motors | Runs camera and AI obstacle detection |
| Executes gait patterns | Plans autonomous route (ROS2) |
| Solves inverse kinematics | Sends movement commands to STM32 |
| Reads PS2 controller (SPI) | Provides SSH access and dashboard |
| Runs safety watchdog and E-stop | Logs telemetry data |
| Communicates with RPi over UART | Communicates with STM32 over UART |

**Key rule:** STM32 + Zephyr is the lead controller — every servo movement passes through 
it. The Raspberry Pi assists and suggests movement but cannot directly drive servos. If the 
Pi disconnects, the STM32 keeps the robot standing safely.

### Zephyr RTOS Thread Structure
| Priority | Thread | Job | Runs Every |
|---|---|---|---|
| 1 (highest) | Servo Output | Send 18 angles to PCA9685 over I2C | 20 ms |
| 2 | Input Handler | Read PS2 joystick and UART commands | 10–20 ms |
| 3 | Gait Engine | Calculate next foot positions | 20 ms |
| 4 | IK Solver | Convert foot positions to joint angles | — |
| 10 (lowest) | Supervisor | Battery monitor, watchdog, USB logging | 100 ms |

The servo output thread runs at the highest priority, guaranteeing servo updates are never 
delayed even under load.

---

## ⚙️ System Workflow
Every movement cycle runs 50 times per second:
1. **User gives input** — PS2 joystick or a UART command from the Raspberry Pi
2. **STM32 reads the input** — decides direction and speed
3. **Gait Engine plans foot positions** — decides which legs swing vs. push
4. **IK calculates joint angles** — closed-form math for each foot's 3D target
5. **STM32 sends angles to the PCA9685 over I2C** — all 18 angles in one burst
6. **Servos move** — the robot steps

---

## 🔩 Servo Control
Servos are driven via PWM through a **PCA9685** PWM driver chip, connected over I2C (SDA + 
SCL) — this offloads the 18 independent PWM channels the STM32 doesn't have pins for. All 
18 angles are sent in a single ~73-byte I2C burst (~1.6 ms).

Right-side legs (4–6) have their coxa direction inverted in firmware, since they're mounted 
as a mirror image of the left-side legs.

---

## 🧮 Inverse Kinematics & Gait
Given a target foot position (x, y, z), closed-form IK (atan2 + law of cosines) computes 
the 3 joint angles — no iterative solving, all 6 legs resolved in under 10 microseconds.

| Gait | Legs Airborne | Speed | Stability | Use Case |
|---|---|---|---|---|
| Tripod | 3 at once | Fast | Good | Default — flat ground |
| Ripple | 2 at once | Medium | Better | Uneven surfaces |
| Wave | 1 at a time | Slow | Maximum | Rough terrain / climbing |

**Tripod gait:** legs are split into Group A (1, 4, 5) and Group B (2, 3, 6). Group A swings 
forward while Group B pushes the body forward, then they swap — each using a smooth 
half-arc foot trajectory.

---

## 🔋 Power System
Powered by a 3S LiPo battery (11.1V), split into two independent rails via separate buck 
converters — servo and logic power are never shared, since motor current spikes can crash 
the electronics.

| Rail | Voltage | Min Current | Supplies |
|---|---|---|---|
| Logic rail | 5V | 3A | Raspberry Pi 4, STM32 Nucleo, PCA9685, PS2 receiver |
| Servo rail | 6V | 10A | All 18 servo motors |

All grounds meet at a single star point. A 4×1000µF capacitor bank on the servo rail 
prevents current-spike resets, and protection includes a 10A fuse, TVS diode for back-EMF, 
and low-battery cutoff at 9.9V.

---

## 🎮 PS2 Controller
A synchronous serial interface (250 kHz, similar to SPI), polled 100 times/sec, running at 
3.3V (no level shifting needed). Analog mode must be explicitly enabled on startup 
(command `0x44`) or joystick axes won't respond.

| Input | Action |
|---|---|
| Left stick | Walk direction (forward/back/strafe) |
| Right stick X | Rotate body in place |
| Triangle | Stand up (neutral position) |
| Cross | Sit down (safe resting position) |
| Square | Tripod gait (fast) |
| L1 | Wave gait (slow, stable) |
| R1 | Increase step size |
| Circle | Decrease step size |
| Start | Emergency stop |
| Select | Hand control to autonomous mode (Raspberry Pi) |

---

## 💻 Software Layout

### STM32 / Zephyr Firmware

```
hexapod-firmware/
├── prj.conf                 # enables I2C, SPI, UART, FPU
├── src/
│   ├── main.c                # creates threads, starts timer
│   ├── drivers/
│   │   ├── pca9685.c          # I2C burst write to PWM chip
│   │   ├── ps2.c              # SPI polling, analog-enable
│   │   └── comms.c            # UART framing, CRC8 check
│   ├── control/
│   │   ├── ik.c                # closed-form IK math
│   │   ├── gait.c              # tripod / ripple / wave
│   │   └── servo_mgr.c         # angle limits, channel map
│   └── app/
│       ├── hexapod.c           # state: IDLE / MANUAL / AUTO
│       └── safety.c            # watchdog, low-battery
```

### Raspberry Pi / ROS2

```
hexapod-rpi/
├── ros2_ws/src/
│   ├── hexapod_bridge/       # UART <-> ROS2 message bridge
│   ├── vision_node/          # camera + obstacle detection
│   └── nav_node/             # path planning, sends commands to STM32
├── scripts/
│   ├── calibrate.py           # per-servo trim offset tool
│   └── tune_gait.py           # live gait parameter tuning
└── config/
    └── robot.yaml             # leg lengths, servo limits
```

---

## 🧪 Testing & Debugging
A strict 7-step bring-up sequence — each step must pass before moving to the next:

1. Bench power only (no battery) — confirm STM32 programming works
2. Single servo test — sweep 0–180°, verify PWM on oscilloscope
3. All 18 channels at neutral — confirm no jitter
4. PS2 controller test — confirm joystick + button input
5. IK math check — known inputs should give ~90/90/90° at home position
6. Single leg gait — one leg swings a full clean cycle
7. Full tripod gait — all 6 legs, start small, increase step size gradually

| Symptom | Likely Cause | Fix |
|---|---|---|
| Servos twitch/jitter | I2C noise / long cables | Shorten cables, add 100nF cap at PCA9685 VCC, use 2.2k pull-ups |
| STM32 resets while walking | 6V rail droops from inrush | Add 4×1000µF caps on 6V rail, check star-point ground |
| Robot walks in circles | Servo trim not calibrated | Run `calibrate.py`, verify measured leg lengths |
| UART errors (RPi ↔ STM32) | Ground offset between boards | Check star-point ground, reduce baud rate |
| IK produces wrong angles | Target outside reachable range | Add boundary check: reject target if D > femur+tibia |
| Servos don't move at all | PCA9685 OE pin not pulled low | Wire OE to GND or pull low in firmware init |
| PS2 joystick stuck at center | Analog mode not enabled | Send analog-enable command (`0x44`) on startup |

---

## 🧑‍🤝‍🧑 Team Structure
| Role | Focus | Deliverable |
|---|---|---|
| Lead | STM32 + Zephyr firmware, RTOS threads, UART protocol | All 18 servos update at stable 50Hz, no jitter |
| Hardware & Power | Chassis assembly, power board, wiring | Robot powered on, both rails stable under full load |
| IK & Gait | Kinematics, gait state machines | Robot walks forward/backward/turns under PS2 |
| Raspberry Pi & AI | ROS2, camera, obstacle detection | Camera detects obstacle, RPi signals stop to STM32 |
| Communication & Integration | UART link, calibration tooling | End-to-end: RPi command → correct servo movement |
| Testing & Debugging | Bring-up checklist, issue tracking, demo | Complete working system demonstrated |

---

## 📅 Execution Plan
Five phases over 9 weeks, each ending in a live physical gate review:

1. **Hardware Setup** (Weeks 1–2) — chassis, power board, servo bring-up
2. **Servo Control** (Weeks 2–3) — PCA9685 driver, RTOS threads, PS2 driver
3. **IK Implementation** (Weeks 3–4) — closed-form solver, verified against known positions
4. **Walking** (Weeks 4–6) — tripod/ripple/wave gaits, tuned on multiple surfaces
5. **AI Integration** (Weeks 6–9) — ROS2, obstacle detection, full autonomous demo

---

## ⚠️ Status
Actively under development. Current focus: integrating the custom PCB into the hardware 
stack and validating gait behavior in ROS/Gazebo simulation ahead of hardware deployment.
