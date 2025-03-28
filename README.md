<a name="readme-top"></a>


<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/DalFSAE/high-voltage-controller">
    <img src="images/dms_logo.jpg" alt="Logo" width="500" height="200">
  </a>

<h3 align="center">DalFSAE High Voltage Controller</h3>
  <p align="center">
    STM32-based firmware for the Driver Monitoring System's High Voltage Controller.
    Developed by the Dalhousie Formula SAE EV Team.
    <br />
    <a href="https://github.com/DalFSAE/high-voltage-controller/"><strong>Explore the project »</strong></a>
    <br />
  </p>
</div>

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running, follow these steps.

### Prerequisites

- **VS Code** with extensions:
  - [STM32 VS Code Extension](https://marketplace.visualstudio.com/items?itemName=stmicroelectronics.stm32-vscode-extension)
  - [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug)
  
- **STM32Cube Tools**:
  - [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
  - [STM32CubeCLT](https://www.st.com/en/development-tools/stm32cubeclt.html)

- **Arm GNU Toolchain**:
  - [Download Link](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

- **CMake** and **Ninja**:
```sh
  # On Ubuntu
  sudo apt install cmake ninja-build

  # On Windows (via Chocolatey)
  choco install cmake ninja
```

### Project Structure 

```sh
Core/                 → Application source and user code
  /Inc                → SMT32CubeMX generated .h headers 
  /Src                → SMT32CubeMX generated .c source
  /Test               → !! Project firmware tests
  /User               → !! Project source code
Drivers/              → CMSIS + HAL drivers
build/                → Build artifacts (generated)
cmake/                → Toolchain and CMake support
CMakeLists.txt        → CMake project root
DMS-High-Voltage-Controller.ioc → STM32CubeMX config
```


### Building

Run the following commands from the project root: 
```sh 
cmake -G Ninja -B build
cmake --build build
```

This will generate the firmware binary:
```sh
build/DMS-High-Voltage-Controller.elf
```

To remove all build artifacts:
```sh
rm -rf build         # On macOS/Linux
Remove-Item -Recurse -Force build  # On PowerShell
```


### Flashing the Firmware

Use this command to build and flash the firmware via ST-Link:

```sh
cmake --build build --target flash
```
Make sure the STM32CubeProgrammer CLI is installed and available on your system PATH. You can override the path like this:
```sh
cmake -DSTM32_PROGRAMMER_CLI="C:/ST/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe" -B build
```


<!-- CONTRIBUTING -->
## Contributing

1. Create a new branch for your feature (`git checkout -b dev/feature`)
2. Commit your Changes (`git commit -m 'Added my feature'`)
3. Push to the Branch (`git push origin dev/feature`)
4. Open a Pull Request against `dev`

<p align="right">(<a href="#readme-top">back to top</a>)</p>


### License 

This project is licensed under the [MIT License](LICENSE).  
© 2025 Dalhousie FSAE EV Team