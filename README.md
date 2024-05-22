# What is it?

Stompbox is a guitar amplification and effects application. It is designed to run either under [jack](https://github.com/jackaudio) or as an audio plugin.

This repository is the core codebase. You can find more information about it in the front-end repository [here](https://github.com/mikeoliphant/StompboxUI).

I run it as both a VST3 plugin in a DAW on my PC, and on my Raspberry Pi pedalboard.

Here is a video of it running on my pedalboard:

[![Neural Amp Modeler on Raspberry Pi](https://img.youtube.com/vi/2I_bxxzQs2s/0.jpg)](https://www.youtube.com/watch?v=2I_bxxzQs2s)

### Building in Linux

First clone the repository:
```bash
git clone --recurse-submodules -j4 https://github.com/mikeoliphant/stompbox
cd stompbox/build
```

Then compile the plugin using:

```bash
cmake .. -DCMAKE_BUILD_TYPE="Release"
make -j4
```

After building, the binary will be in build/stompbox-jack.

