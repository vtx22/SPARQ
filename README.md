[![SPARQ (Windows)](https://github.com/vtx22/SPARQ/actions/workflows/build_sparq_windows.yaml/badge.svg)](https://github.com/vtx22/SPARQ/actions/)

# SPARQ
_SPARQ is a serial plotter with a focus on fast realtime plotting and the display of big datasets in different plot types_


![sparq](img/sine_anim.gif)


## Main Features
- Realtime plotting of serial data
- Simultaneous display of 256 independent datasets
- Different plot types like Line, Bar, Heatmap, XY
- Many plot customizations 
- Measure markers
- CSV export
- Display of big datasets using downsampling (ToDo)
- Math functions (ToDO)
- Handling of different data types and message formats (float, int, uint, sample by sample, bulk, strings)


## Installation
Download the newest version for your operating system from the [Releases](https://github.com/vtx22/SPARQ/releases) and open the executable.

## Usage
Open a COM port with the correct baud rate and transmit data from a place of your choice, for example a microcontroller.
Currently only data transmitted in the custom SPARQ format can be interpreted by the plotter. An ASCII mode is planned and will be implemented in the future.

The custom SPARQ format is documented in this Readme. Furthermore, there are custom SPARQ Sender (SPARQS) classes in their own repository for STM32. A general base class will be available in the future.


## Credits :heart:
This software was built using: [ImGui](https://github.com/ocornut/imgui) | [ImPlot](https://github.com/epezent/implot) | [ImGuiNotify](https://github.com/TyomaVader/ImGuiNotify) | [SFML](https://github.com/SFML/SFML) | [ImGui-SFML](https://github.com/SFML/imgui-sfml)
