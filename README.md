GDModdingTool is a tool for modding Grim Dawn with two main goals:
* Make modding, changing thousands of game files, easy and fast
* Make it affected less, preferably none at all, by game updates (Whenever game gets an update, just re-running tool should be enough)

See all possible customizations [Here](https://github.com/azakhi/GDModdingTool/blob/master/Example%20Configurations/config_ALL.txt)

## Installation
#### Download
Latest release can be downloaded [Here](https://github.com/azakhi/GDModdingTool/releases/latest)

#### One Time Source File Setup
Tool requires `.dbr` files used by game. `ExtractFiles.bat` file can be used to extract all required files. Run this file and enter required paths when promted.

If you prefer extacting files yourself, following commands can be used to extract base game and expansion files:
```
path\to\game\ArchiveTool.exe path\to\game\database\templates.arc -extract "path\to\game\database"
path\to\game\ArchiveTool.exe path\to\game\database\database.arz -database "path\to\output\folder"
path\to\game\ArchiveTool.exe path\to\game\gdx1\database\GDX1.arz -database "path\to\output\folder"
path\to\game\ArchiveTool.exe path\to\game\gdx2\database\GDX2.arz -database "path\to\output\folder"
```

#### Optional New Mod Creation (Recommended)
Run `AssetManager.exe` that can be found in game folder and chose `Work Offline`. From top menu select `Mod > New...` and create a new mod. Use this mod's `database\records` folder as program output.

#### Program Configuration
Copy `config_ALL.txt` file that can be found in `Example Configurations` folder to the program folder and rename to `config.txt`. This file contains all required fields and explanations of them. Change required fields before running program.

## Usage
Desired customizations can be defined in `config.txt` file. Also, example config files can be copied from `Example Configurations` and be used after changing required paths.

After config file is ready, program can be run by simply double clicking on it. It will make changes and save changed files to given folder. Those files now can be used in any way desired.

#### Building Custom Mod
If you have followed `Optional New Mod Creation (Recommended)` and want to build your custom mod, run `AssetManager.exe` again. From top menu select `Mod > Select > YourModName`. Then from top menu select `Build > Build`. After build is completed, mod can be chosen in game from `Custom Game`.

#### Merging With Other Mods
If you want to merge with other mods, do so before building. You can simply copy source of other mods and paste them into your custom mod folder. Then mod can be built similarly.
