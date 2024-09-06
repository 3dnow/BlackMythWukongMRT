# BlackMythWukongMRT
Memory Researching Tool for BlackMythWuKong

## Description
This tool is designed for advanced memory analysis and modification of BlackMythWuKong(b1). It allows users to scan, locate, and modify specific memory addresses within the target b1 process.

## Features
- Process memory scanning
- Value-based memory search
- Iterative filtering of search results
- Memory modification capabilities
- Module information retrieval

## Disclaimer
This tool is intended for educational and research purposes only. Use of this tool to modify commercial games or software may violate terms of service or local laws. The authors are not responsible for any misuse or damages caused by this tool.

## Usage
There are two modes of operation:

a. Memory Scanning Mode (Default):
   - Enter the first value to scan for when prompted.
   - The tool will scan the memory of the b1 process.
   - After the first scan completes, it will show the number of matches found.
   - Enter the second value to scan for when prompted.
   - The tool will narrow down the results based on the second value.
   - Final matches will be displayed with their memory addresses and associated module names.

b. Memory Modification Mode (use -m flag):
   - Run the tool with the "-m" flag (e.g., `tool.exe -m`).
   - Enter the memory address (in hexadecimal) you want to modify.
   - Enter the new 32-bit value (in decimal) to write to that address.
   - The tool will attempt to modify the memory and report success or failure.
   - Press Enter without inputting an address to exit the modification mode.

The tool will automatically exit after completing the scan in the default mode. In the modification mode, you can exit by pressing Enter without inputting an address.


## How to Use against b1
1. Scanning and Modifying "Will" (灵蕴):
   - Perform an initial scan with your current "Will" value.
   - Gain some "Will" in the game.
   - Conduct a second scan with the new value.
   - This should yield two addresses.
   - Use the -m flag to modify these addresses to your desired values.

2. Scanning and Modifying "Spark" (灵光点):
   - Locate your current Spark value on the character control panel (displayed as XXXX/YYYY, where XXXX is your target).
   - Perform an initial scan with this value.
   - Slightly change your Spark value in-game.
   - Conduct a second scan with the updated value.
   - Note: You may get numerous results due to the potentially low value.
   - Compare the results from both scans to identify approximately two unique addresses.
   - These addresses correspond to the "Spark" values.
   - Use the -m flag to modify these addresses to your desired values.
   - Return to the game and gain a small amount of Spark.
   - You should now see your modified Spark value reflected in-game.

You can explore additional modifications and values within the game on your own. However, the methods described above are sufficient for me to completing the game.

## Requirements
- Windows operating system
- Visual Studio 2019 or later (for compilation)

## Contributing
Contributions, issues, and feature requests are welcome. Feel free to check issues page if you want to contribute.

All code and this readme are written by Claude 3.5-sonnet.

