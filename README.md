# Differential Drive Mobile Robot & Line Following PID Simulation

![C](https://img.shields.io/badge/Language-C-blue.svg)
![OpenGL](https://img.shields.io/badge/Graphics-OpenGL%2FGLUT-red.svg)
![Control](https://img.shields.io/badge/Control-PID-brightgreen.svg)
![Domain](https://img.shields.io/badge/Domain-Mobile%20Robotics-orange.svg)

## Overview

This repository contains a C-based 3D simulation of a **Differential Drive Mobile Robot** functioning as an autonomous Line Follower. Built with **OpenGL (GLUT)**, the simulation models robot kinematics, virtual infrared (IR) sensor array processing, and closed-loop PID control for trajectory correction.

---

## Technical Features

### 1. Differential Kinematics Model
- Employs a Jacobian matrix to map independent left/right wheel velocities ($dq_1, dq_2$) to the robot's Cartesian pose ($x, y$) and heading angle ($\psi$).
- Dynamic pose updating and transformation matrices for realistic 3D movement.

### 2. Virtual Vision & Sensor Array
- Uses OpenGL pixel reading (`glReadPixels`) from a downward-facing virtual camera to simulate an **8-channel IR sensor array**.
- Texture mapping (`loadGLTexture`) overlays custom `.ppm` track images onto the simulation floor.

### 3. Multi-Window Rendering Architecture
The simulation concurrently renders four distinct perspectives using GLUT multi-window management:
1. **Main View:** Isometric tracking of the robot.
2. **Back-Top Camera:** Third-person trailing perspective.
3. **Sensor Camera:** Downward-facing orthographic projection acting as the physical sensor input.
4. **IR Raw Output:** 2D pixel-mapped view of the digitized IR sensor readings.

### 4. PID Steering Control Modes
- **Servo Mode:** Direct angular position command targeting based on sensor error.
- **PWM Mode:** Duty-cycle simulated control mapping sensor error to pseudo-voltage ($\theta_{motor}$ ratio), feeding wheel velocity differentials.

---

## Compilation & Execution

Requires a C compiler (GCC) with **OpenGL/GLUT** installed under a POSIX-compliant environment (Cygwin or Linux).

1. Ensure the track texture files (`.ppm`) are located in the working directory (or update the file path in the source code).
2. Compile the source:
   ```bash
   gcc -o mobile_sim src/mobile_robot_sim.c -lglut -lGLU -lGL -lm
