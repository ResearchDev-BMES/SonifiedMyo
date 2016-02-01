# SonifiedMyo

## Texas Instruments TIVA C Series TM4C123GXL Sample Code: music_maker
#### Import the project
1. Project -> Import CCS Project
2. Select search-directory -> Browse
3. Remember to check "Copy projects into workspace"

#### Configure the project
1. Right click music_maker -> Properties
2. Resources -> Linked Resources -> SW_ROOT
3. Change SW_ROOT to the directory that you install the TivaWare. <br />
(Default Location: C:\TexasInstruments\TivaWare_C_Series-2.1.2.111)

#### Build the project
1. Right click music_maker -> Build Configurations -> Build All
2. Run -> Debug

### Pin Configuration

TIVA    -->    VS1053<br />
5V      -->    5V<br />
GND     -->    GND<br />
PB1     -->    D2<br />
PE4     -->    D9<br />
PE5     -->    Output of Potentiometer (Must be less than 3.3V) <br />
