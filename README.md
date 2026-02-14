# rangers-sdk example

This is an example starter project using the rangers-sdk to build your own DLL mods.

## Setting up the development environment

You will need to have the following prerequisites installed:

* Visual Studio 2026

Check out the project and make sure to also check out its submodules:

```sh
git clone --recurse-submodules https://github.com/HE2-SDK/rangers-sdk-example.git
```

* "Open Folder" in Visual Studio and select the folder where you extracted the project.
* Right click the top level CMakeLists.txt in the solution explorer and select "Set as Startup Item".
  If this option does not show up, try the "Configure [some name]" option first.
* Select the "x64 Debug" configuration and build the project.
* In the debug menu, select "rsdk-example.dll (Install)", and try to launch the project.
  It will fail, but open a launch settings file. Replace the content of this file with the following:

```json
{
  "version": "0.2.1",
  "defaults": {},
  "configurations": [
    {
      "type": "dll",
      "exe": "C:\\Program Files (x86)\\Steam\\steamapps\\common\\SonicFrontiers\\SonicFrontiers.exe",
      "program": "C:\\Program Files (x86)\\Steam\\steamapps\\common\\SonicFrontiers\\SonicFrontiers.exe",
      "cwd": "C:\\Program Files (x86)\\Steam\\steamapps\\common\\SonicFrontiers",
      "currentDir": "C:\\Program Files (x86)\\Steam\\steamapps\\common\\SonicFrontiers",
      "project": "CMakeLists.txt",
      "projectTarget": "rsdk-example.dll (Install)",
      "name": "rsdk-example.dll (Install)"
    }
  ]
}
```

* Now retry