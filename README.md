
---

# Traction Control Simulation

This repository demonstrates a simple **traction control** simulation using C++ and [SDL2](https://www.libsdl.org/) for visualization. The project models a vehicle with four wheels, each of which can experience **slip** if the wheel’s angular velocity diverges from the vehicle’s linear speed. A **TractionControl** system then attempts to keep the slip ratio within a desired range by adjusting brake torque or drive torque. I'm not very good at visualisation so sorry if it is not the best. The code is commented allowing everyone to change the parameters.

## Features

1. **Vehicle & Wheel Modeling**  
   - Each wheel has rotational inertia, angular velocity, brake torque, and drive torque.  
   - The vehicle has a linear speed and applies friction forces based on wheel slip.

2. **Simple Traction Control**  
   - The control logic monitors wheel slip.  
   - If slip exceeds a threshold, it ramps up brake torque and reduces drive torque.  
   - If slip is too low, it ramps down brake torque and increases drive torque.

3. **SDL2 Visualization**  
   - Displays a **top-down view** of the car (gray rectangle) and wheels (white rectangles).  
   - Renders **bar graphs** showing:
     - Green bar: vehicle’s forward speed  
     - Blue bars: each wheel’s linear speed  
     - Red bars (on the right): slip ratio for each wheel  

4. **Fixed-Timestep Physics**  
   - A stable loop that updates physics at 100 Hz, then renders at ~30 FPS for smoother visuals.

---

## Physics

This simulation is intentionally **simplified** but illustrates the core ideas of vehicle dynamics and traction control. Key equations include:

1. **Net Force & Acceleration**  

   The sum of friction forces from all wheels determines the net force on the vehicle:
   $$
   F_\text{net} = \sum_{i=1}^n F_{\text{friction},i}
   $$

   Then, by Newton’s second law:
   $$
   a = \frac{F_\text{net}}{m},
   \quad
   v(t+\Delta t) = v(t) + a \,\Delta t.
   $$

2. **Slip Ratio**  

   Let \(v\) be the vehicle’s linear speed (m/s) and $\omega$ the wheel’s angular velocity (rad/s). If the wheel radius is \(r\), the **wheel’s linear speed** is $\omega \, r$. The **slip ratio** is:
   $$
   \text{slip} = \frac{\omega \, r - v}{\max(v, \epsilon)}.
   $$

   A positive slip indicates the wheel is spinning faster than the ground speed, while a negative slip indicates it’s rotating slower (potentially locking up).

3. **Friction Model**  

   We use a simple exponential approach to approximate the friction coefficient $\mu$ growing with slip magnitude:
   $$
   \mu(\text{slip}) = \mu_\text{peak}
       \bigl(1 - e^{-k \,\lvert\text{slip}\rvert}\bigr),
   $$
   where \(k\) is a tuning constant (e.g., 10), and \(\mu_\text{peak}\) is the maximum friction coefficient.  
   
   The friction force from each wheel is:
   $$
   F_\text{friction} = \mu(\text{slip}) \cdot N,
   $$
   where \(N\) is the normal force on that wheel.

4. **Wheel Dynamics**  

   Each wheel has rotational inertia \(I\). Net torque:
   $$
   \tau_\text{net} = \tau_\text{drive} \;-\; \tau_\text{brake} \;-\; \tau_\text{friction},
   $$
   and
   $$
   \alpha = \frac{\tau_\text{net}}{I},
   \quad
   \omega(t+\Delta t) = \omega(t) + \alpha \,\Delta t.
   $$

---

## Building

This project uses [CMake](https://cmake.org/) and depends on **SDL2**. The folder structure is something like:

```
├── include/
│   ├── Vehicle.h
│   ├── TractionControl.h
│   ├── Simulation.h
│   ├── Visualizer.h
├── src/
│   ├── Vehicle.cpp
│   ├── TractionControl.cpp
│   ├── Simulation.cpp
│   ├── Visualizer.cpp
│   ├── main.cpp
├── SDL2/
│   ├── include/
│   ├── lib/
│   ├── SDL2.dll
└── CMakeLists.txt
```

1. **Install / Provide SDL2**  
   - Make sure to install SDL2 with the `SDL2/` folder with headers, libraries, and `SDL2.dll`.  
   - Adjust paths in `CMakeLists.txt` so that `SDL2_INCLUDE_DIR`, `SDL2_LIBRARY`, and `SDL2_MAIN_LIBRARY` point to your SDL2 installation.

2. **Configure & Build**  
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
   On Windows with MSYS2 or similar, the commands are similar (e.g., `mingw32-make`).

3. **Run the Program**  
   - On Windows:
     ```bash
     ./traction_control.exe
     ```
   - On Linux / macOS:
     ```bash
     ./traction_control
     ```

If you see a window with a gray rectangle (the car) and four white rectangles (the wheels) plus colored bars, everything is working!

---

## Usage

Once launched, a window will appear with:

- **Car body** (gray rectangle) in the center.  
- **Wheels** (white rectangles) at each corner.  
- **Green bar** (on the left) showing the vehicle’s linear speed.  
- **Blue bars** (beside the green) showing each wheel’s speed.  
- **Red bars** (on the right) showing the slip ratio for each wheel.  

Close the window to exit.
