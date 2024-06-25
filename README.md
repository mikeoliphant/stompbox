# What is it?

Stompbox is a guitar amplification and effects library.

This repository is the core codebase. It can be run as a headless [jack](https://github.com/jackaudio) client using the stompbox-jack application. 

[StompboxUI](https://github.com/mikeoliphant/StompboxUI) is the front-end GUI repository for stompbox. It can be used as a remote application to connect and control stompbox-jack, or it can be built along with the core code into a VST3 plugin.

I run it as both a VST3 plugin in a DAW on my PC, and on my Raspberry Pi pedalboard.

Here is a video of it running on my pedalboard:

[![Neural Amp Modeler on Raspberry Pi](https://img.youtube.com/vi/2I_bxxzQs2s/0.jpg)](https://www.youtube.com/watch?v=2I_bxxzQs2s)

This is the hardware I'm using:
- Raspberry Pi 4
- Hotone Jogg audio interface
- Hotone Ampero Control MIDI Controller
- Wio Terminal (used for a display using my [SerialTFT](https://github.com/mikeoliphant/SerialTFT) app)

# Building

First clone the repository:
```bash
git clone --recurse-submodules -j4 https://github.com/mikeoliphant/stompbox
```

## Building stompbox-jack in Linux

To build the stombox-jack client, you need to have the jack development libraries installed:

```bash
sudo apt-get install libjack-jackd2-dev
```

Then compile the plugin using:

```bash
cd stompbox/build
cmake .. -DCMAKE_BUILD_TYPE="Release"
make
```

## Building stompbox-jack in Windows

You need to have jack for windows installed:

https://jackaudio.org/downloads/

Configure the stompbox build with cmake:

```bash
cd stompbox/build
cmake.exe -G "Visual Studio 17 2022" -A x64 ..
```

This will get you a Visual Studio solution with projects for the core library and stompbox-jack.

# Running stompbox-jack

After building, the binary will be in build/stompbox-jack.

You must have [jack](https://github.com/jackaudio) installed and configured to run on your audio device. If jack is running (as the same user you run stompbox with), the running instance will be used, otherwise jack will be started using your default configuration.

stompbox-jack takes a single optional command-line argument - the name of the preset to start with.

Currently it will connect to the first available jack audio input, and the first two available jack audio outputs. It will also connect to any available jack midi inputs.

The following folders are epected to exist in the same folder as the stompbox-jack executable:

**Presets** - folder to store configuration presets

**NAM** - folder containing .nam amp models

**Cabinets** - folder containing .wav format cabinet impulse responses

**Reverb** - folder containing .wav format convolution reverb files

**Music** - folder containing .wav audio files for the built-in audio looper

### How to Configure Presets?

In order to configure presets, you need to use the remote user-interface in the [StompboxUI](https://github.com/mikeoliphant/StompboxUI) repository.

If you have a Windows machine, you can grab the StompboxRemote application from the [the latest release](https://github.com/mikeoliphant/StompboxUI/releases/latest).
