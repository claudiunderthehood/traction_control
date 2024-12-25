# Traction Control Simulation

This repository demonstrates a simple **traction control** simulation using C++, SDL for visualization and libtorch. The project offers two kinds of emulation. The first one models a vehicle with four wheels, each of which can experience **slip** if the wheel’s angular velocity diverges from the vehicle’s linear speed. A **TractionControl** system then attempts to keep the slip ratio within a desired range by adjusting brake torque or drive torque. The second one offers a **traction control simulation powered by a ML model** trained to predict best brake and drive torque to reduce wheels slip. I'm not very good at visualization, so sorry if it is not the best. The code is commented, allowing everyone to change the parameters.

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

5. **AI-Enhanced Traction Control**  
   - Uses a **Multi-Layer Perceptron (MLP)** trained with PyTorch to optimize slip ratio control.  
   - The MLP replaces the manual control logic, dynamically predicting optimal brake and drive torque values.  
   - Integration of AI models through [libtorch](https://pytorch.org/cppdocs/).

---

## Physics

This simulation is intentionally **simplified** but illustrates the core ideas of vehicle dynamics and traction control. Key equations include:

1. **Net Force & Acceleration**  

   The sum of friction forces from all wheels determines the net force on the vehicle:  
   ![\Large F_\text{net} = \sum_{i=1}^n F_{\text{friction},i}](https://latex.codecogs.com/svg.latex?\color{white}\Large&space;F_\text{net}=\sum_{i=1}^n&space;F_{\text{friction},i})  

   Then, by Newton’s second law:  
   ![\Large a = \frac{F_\text{net}}{m}, \quad v(t+\Delta t) = v(t) + a \, \Delta t.](https://latex.codecogs.com/svg.latex?\color{white}\Large&space;a=\frac{F_\text{net}}{m},\quad&space;v(t+\Delta&space;t)=v(t)+a\,\Delta&space;t.)

2. **Slip Ratio**  

   Let \(v\) be the vehicle’s linear speed (m/s) and \(\omega\) the wheel’s angular velocity (rad/s). If the wheel radius is \(r\), the **wheel’s linear speed** is \(\omega \, r\). The **slip ratio** is:  
   ![\Large \text{slip} = \frac{\omega \, r - v}{\max(v, \epsilon)}](https://latex.codecogs.com/svg.latex?\color{white}\Large&space;\text{slip}=\frac{\omega\,r-v}{\max(v,\epsilon)})

   A positive slip indicates the wheel is spinning faster than the ground speed, while a negative slip indicates it’s rotating slower (potentially locking up).

3. **Friction Model**  

   We use a simple exponential approach to approximate the friction coefficient \(\mu\) growing with slip magnitude:  
   ![\Large \mu(\text{slip}) = \mu_\text{peak} \bigl(1 - e^{-k \, \lvert\text{slip}\rvert}\bigr)](https://latex.codecogs.com/svg.latex?\color{white}\Large&space;\mu(\text{slip})=\mu_\text{peak}\bigl(1-e^{-k\,\lvert\text{slip}\rvert}\bigr))

   The friction force from each wheel is:  
   ![\Large F_\text{friction} = \mu(\text{slip}) \cdot N](https://latex.codecogs.com/svg.latex?\color{white}\Large&space;F_\text{friction}=\mu(\text{slip})\cdot&space;N)

   where \(N\) is the normal force on that wheel.

4. **Wheel Dynamics**  

   Each wheel has rotational inertia \(I\). Net torque:  
   ![\Large \tau_\text{net} = \tau_\text{drive} - \tau_\text{brake} - \tau_\text{friction}](https://latex.codecogs.com/svg.latex?\color{white}\Large&space;\tau_\text{net}=\tau_\text{drive}-\tau_\text{brake}-\tau_\text{friction})  

   and  
   ![\Large \alpha = \frac{\tau_\text{net}}{I}, \quad \omega(t+\Delta t) = \omega(t) + \alpha \, \Delta t.](https://latex.codecogs.com/svg.latex?\color{white}\Large&space;\alpha=\frac{\tau_\text{net}}{I},\quad\omega(t+\Delta&space;t)=\omega(t)+\alpha\,\Delta&space;t.)

---

## AI Integration

### **Dataset Generation Criteria**
The training dataset for the AI model was generated using a simulated vehicle under various conditions which were taken from the standard emulation:
1. **Input Parameters**:
   - Wheel slip ratios ranging from \(-0.5\) to \(0.5\).
   - Vehicle linear speeds between \(0\) m/s and \(30\) m/s.
   - Brake and drive torque values sampled uniformly from \(0\) to \(1000\) Nm.
2. **Output Labels**:
   - Optimal brake and drive torques calculated to maintain a slip ratio between \(-0.1\) and \(0.1\).

The data has been cleansed and new features have been created to better help the model catch patterns between the data. See ```data_analysis.ipynb```.

### **Model Used**
- **Architecture**: Multi-Layer Perceptron (MLP) using Linear Regressor.
  - Input: [Slip Ratio, Vehicle Speed]
  - Hidden Layers: Two layers with 128 neurons each, using ReLU activation.
  - Output: [Brake Torque, Drive Torque]
- **Training**: Performed in Python using PyTorch.
  - Loss Function: Mean Squared Error (MSE)
  - Optimizer: Adam
  - Epochs: 50

After the training and evaluation phase, the model has been tested with unseen data in ```test_model.ipynb```.

### **Integration**
The trained PyTorch model was exported to the TorchScript format (`mlp_model_traced.pt`) and loaded into the simulation using **libtorch**.

---

## Directory Tree

```
├── ai-emulation/
│   ├── datasets/
│   ├── modules/
│   ├── traction_control_model/
│   │   ├── include/
│   │   ├── libtorch/
│   │   ├── SDL2/
│   │   ├── src/  
│   │   ├── CMakeLists.txt
│   │   └── mlp_model_traced.pt
│   ├── data_analysis.ipynb
│   ├── test_model.ipynb
│   └── train_model.py
├── emulation/
│   ├── include/
│   ├── SDL2/
│   └── src/
├── .gitignore
├── README.md
└── requirements.txt
```

---

## Building the standard emulation

This project uses **CMake 3.27** and depends on **SDL2**.

1. **Install Dependencies**
   - Navigate into the ```emulation``` directory, Ensure **SDL2** is properly installed and their paths are set in `CMakeLists.txt`. On Windows, put the SDL2 folder inside the emulation directory.

2. **Configure & Build**
   ```bash
   cmake . -G "Unix Makefiles" -B build -DBUILD_MAIN=ON   
   cd build
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

---

## Building the AI emulation

This project uses **CMake 3.27** and depends on **SDL2** and **libtorch**. Pay attention that your CUDA version and libtorch version must match in order for this to properly work. Additionally, on Windows you should download Visual Studio Build Tools in order to get MSVC. The model is already inside the traction_control_model directory.

1. **Install Dependencies**
   - Navigate into the ```ai-emulation/traction_control_model``` directory, Ensure **SDL2** and **libtorch** are properly installed and their paths are set in `CMakeLists.txt`. On Windows, put the SDL2 and libtorch folders inside the transaction_control_model directory. **If you are using CUDA >= 12 then nvToolsExt will not be shipped with the CUDA Toolkit. Download CUDA 11.8, select custom installation, uncheck everything and only check NSIGHT NVTX and install it. When it finishes navigate to the NVTX installation and move nvToolsExt64_1.lib into C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/vX.Y/lib/x64/.**

2. **Configure & Build on Windows**
   ```bash
   cmake -G "Visual Studio 17 2022" -A x64 -B build -DCMAKE_BUILD_TYPE=Release  -DCMAKE_PREFIX_PATH="C:\path\to\libtorch" -DCMAKE_GENERATOR_TOOLSET="cuda=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\vX.Y" -DCUDA_nvToolsExt_LIBRARY="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/vX.Y/lib/x64/nvToolsExt64_1.lib" .  
   cmake --build build --config Release  
   ```

3. **Configure & Build on Linux or macOS**
   ```bash
   cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="/path/to/libtorch"

   cmake --build build  
   ```

4. **Run the Program on Windows**: Navigate into ```build/Release/``` and run the **traction_control.exe** file.


5. **Run the Program on Linux**: Navigate into ```build``` and run:
   ```bash
   ./traction_control
     ```
---


## Usage

Once launched, a window will appear with:

- **Car body** (gray rectangle) in the center.  
- **Wheels** (white rectangles) at each corner.  
- **Green bar** (on the left) showing the vehicle’s linear speed.  
- **Blue bars** (beside the green) showing each wheel’s speed.  
- **Red bars** (on the right) showing the slip ratio for each wheel.

The standard emulation will be smoother and faster. The ai-emulation will be laggy and
will skip some frames due to CUDA/SDL not synchronising but similarly to the 
standard emulation, after a while the Red bars will diminish proving that the model is doing
correct predictions.

Close the window to exit.