# What is it?

Stompbox is a guitar amplification and effects application. It is designed to run either under [jack](https://github.com/jackaudio) or as an audio plugin.

This repository is the core codebase. You can find more information about it in the front-end repository, [StompboxUI](https://github.com/mikeoliphant/StompboxUI).

I run it as both a VST3 plugin in a DAW on my PC, and on my Raspberry Pi pedalboard.

Here is a video of it running on my pedalboard:

[![Neural Amp Modeler on Raspberry Pi](https://img.youtube.com/vi/2I_bxxzQs2s/0.jpg)](https://www.youtube.com/watch?v=2I_bxxzQs2s)

### Building in Linux

First clone the repository:
```bash
git clone --recurse-submodules -j4 https://github.com/mikeoliphant/stompbox
cd stompbox/build
```

To build the stombox-jack client, you need to have the jack development libraries installed:

```bash
sudo apt-get install libjack-jackd2-dev
```

Then compile the plugin using:

```bash
cmake .. -DCMAKE_BUILD_TYPE="Release"
make
```

### Running in Linux

After building, the binary will be in build/stompbox-jack.

You must have [jack](https://github.com/jackaudio) installed and configured to run on your audio device. If jack is running (as the same user you run stompbox with), the running instance will be used, otherwise jack will be started using your default configuration.

stompbox-jack takes a single optional command-line argument - the name of the preset to start with.

The following folders are epected to exist in the same folder as the stompbox-jack executable:

**Presets** - folder to store configuration presets

**NAM** - folder containing .nam amp models

**Cabinets** - folder containing .wav format cabinet impulse responses

**Reverb** - folder containing .wav format convolution reverb files

**Music** - folder containing .wav audio files for the built-in audio looper

### How to Configure Presets?

In order to configure presets, you need to use the remote user-interface in the [StompboxUI](https://github.com/mikeoliphant/StompboxUI) repository.

If you have a Windows machine, you can grab the StompboxRemote application from the [the latest release](https://github.com/mikeoliphant/StompboxUI/releases/latest).
