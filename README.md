# Factania
Factania is a DS Factory Builder set in the SHUTTLEFALL universe.  
After a crash landing, it's up to you to scrounge for resources and launch back to the Titania Mothership.

# Build Instructions
For a quick build, run build.bat or build.sh in the scripts directory

## Development environment

### First time setup:
- Install docker
  - It is recommended that you install docker desktop on windows as most configuration is handled for you, including adding docker to the path both on windows and WSL. 
  > [!IMPORTANT]  
  > Ensure you clone using the below command so the SDK is properly initialized.
- Run `git clone --recursive https://github.com/fidg1t/FactoryDS.git`
- Run `scripts/generate_visual_studio.bat` if you want to use visual studio
  - This will generate the project in the `visual_studio` directory. Visual studio will be very confused if you ask to debug any fo the targets, so instead only build.
- Targets
  - `Build_NDS` will create a .nds file in the build directory.
  - `Melon_NDS` will build and run the .nds file in the bundled MelonDS app.
  - `factoryds` is where the sources are contained. In the visual studio project, this is where you edit files. 
