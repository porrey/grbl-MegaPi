![Grbl Logo](https://github.com/gnea/gnea-Media/blob/master/Grbl%20Logo/Grbl%20Logo%20250px.png?raw=true)

# grbl-MegaPi (Makeblock LaserBot Firmware)

[![Latest Stable Release](https://img.shields.io/github/v/release/porrey/grbl-MegaPi?display_name=tag&sort=semver)](https://github.com/porrey/grbl-MegaPi/releases/latest)

**Latest stable for LaserBot:** https://github.com/porrey/grbl-MegaPi/releases/latest

## How to upgrade (quick path)

1. Open the latest stable release:
   - https://github.com/porrey/grbl-MegaPi/releases/latest
2. Download source ZIP, then install the `grbl` folder as an Arduino library.
   - Arduino IDE: **Sketch > Include Library > Add .ZIP Library...**
3. Open **File > Examples > grbl > grblUpload**.
4. Select **Arduino Mega or Mega 2560** and upload to your MegaPi.

> Planned distribution path: Arduino Library Manager support (see `doc/markdown/distribution.md` for submission/release steps).

This repository is a Grbl-Mega fork configured for the **Makeblock LaserBot** and its **MegaPi (ATmega2560)** controller.

The goal is to replace the original legacy firmware/software stack with a standard Grbl workflow so the LaserBot can be used with modern software like **LightBurn**.

> Base Grbl-Mega documentation is available here:  
> https://github.com/gnea/grbl-Mega/blob/edge/README.md

## Makeblock LaserBot + MegaPi

### LaserBot

The Makeblock LaserBot is a 2-axis laser engraver/cutter platform. In this firmware:

- X/Y motion is fully supported with LaserBot pin mapping.
- Laser power uses Grbl spindle/laser control (`M3`, `M4`, `M5`, `S` values).
- Homing and limit behavior are configured for LaserBot endstops.

![Makeblock LaserBot](images/laserbot.png)

### MegaPi board

The MegaPi is based on the **ATmega2560**, so it can be programmed directly from the Arduino IDE as an Arduino Mega-class target.

This fork includes a dedicated MegaPi board map and LaserBot defaults:

- `DEFAULTS_MEGAPI_LASERBOT` enabled in `grbl/config.h`
- `CPU_MAP_2560_MEGAPI_BOARD` enabled in `grbl/config.h`
- LaserBot-specific pin map in `grbl/cpu_map.h`
- LaserBot driver/limits/laser PWM handling in `grbl/megapi.c`

![Makeblock MegaPi](images/megapi.png)

## LaserBot defaults in this firmware

Key machine defaults compiled into this build include:

- Baud: **230400**
- Work area defaults: **X 340 mm**, **Y 360 mm**
- Laser mode enabled by default (`$32=1`)
- Homing enabled by default (`$22=1`)
- PWM range aligned for LaserBot laser control (`S0` to `S255`)

These come from the `DEFAULTS_MEGAPI_LASERBOT` profile in `grbl/defaults.h`.

## Build and flash firmware with Arduino IDE

The LaserBot uses the standard Arduino upload workflow.

### 1) Download this firmware

Either:

- Clone:
  - `git clone https://github.com/porrey/grbl-MegaPi.git`
- Or download ZIP from GitHub and extract it.

### 2) Install the Grbl library into Arduino IDE

1. Copy the repository's `grbl` folder into your Arduino libraries folder.
   - Typical location: `Documents/Arduino/libraries/grbl`
2. Restart Arduino IDE.

### 3) Open the upload sketch

In Arduino IDE:

1. Open **File > Examples > grbl > grblUpload**
2. This loads: `grbl/examples/grblUpload/grblUpload.ino`

### 4) Select board and port

In **Tools**:

- **Board**: `Arduino Mega or Mega 2560`
- **Processor**: `ATmega2560` (if shown)
- **Port**: Select the LaserBot serial port

### 5) Flash firmware

Click the standard **Upload** button in Arduino IDE.

That is all that is required to flash the LaserBot.

## LightBurn setup

After firmware upload, configure LightBurn with the provided device profile.

### 1) Download and install LightBurn

Get LightBurn from: https://lightburnsoftware.com

### 2) Import the LaserBot device profile

1. Open LightBurn.
2. Go to **Devices**.
3. Click **Import**.
4. Select:
   - `LightBurn/Laserbot.lbzip`
5. Finish the import wizard and connect to the LaserBot serial port.

The `Laserbot.lbzip` profile includes the key connection and machine parameters needed for this firmware.

### 3) Open included sample jobs

This repository includes LightBurn samples in `LightBurn/`:

1. **`Cut-Test.lbrn2`**  
   20 cm x 20 cm cut test pattern.
2. **`Test.lbrn2`**  
   Text engraving test file.
3. **`mando.lbrn2`**  
   Image etching example.

## LaserBot support summary (what this fork changes)

This fork adds and enables the pieces needed to run a Makeblock LaserBot as a Grbl laser machine:

1. **LaserBot/MegaPi build profile enabled**
   - `grbl/config.h` enables MegaPi CPU map and LaserBot defaults.
2. **LaserBot hardware pin mapping**
   - `grbl/cpu_map.h` defines LaserBot motion, enable, endstop, and laser PWM pins.
3. **MegaPi board-specific runtime support**
   - `grbl/megapi.c` initializes drivers, reads LaserBot limits, and controls laser PWM behavior.
4. **Laser-oriented defaults**
   - `grbl/defaults.h` sets machine travel, speeds, homing, baud, and laser mode defaults for LaserBot.
5. **Laser command behavior compatibility**
   - Laser power behavior is adapted for MegaPi/LaserBot handling in core motion/spindle flow.

Together, these changes modernize the LaserBot workflow and allow practical use with current host software, especially LightBurn.

## Maintainer discoverability checklist

Repository settings that should be configured on GitHub:

- **Description**: `Makeblock LaserBot firmware upgrade for MegaPi using Grbl + LightBurn workflow`
- **Topics**: `laserbot`, `makeblock`, `megapi`, `grbl`, `firmware`, `lightburn`

See also:

- Release/submission workflow: `doc/markdown/distribution.md`
- Community announcement templates: `doc/markdown/community_announcement.md`
