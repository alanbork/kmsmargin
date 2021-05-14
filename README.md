X11 users can change the overscan margins using xrandr, but if you are running the raspberry pi console without x11 that won't work, and the config.txt method is hard to fine tune. This can be run after boot and allows semi-interactive fine tuning. It works with KMS out of the box, or fkms with a simple comment/uncomment swap.


**What do I need?**

You need a GCC compiler, GDM, EGL, and GLES libraries. The GCC compiler is already included in the Raspbian image. To install the other libraries, simply run:

```bash
sudo apt-get install libegl1-mesa-dev libgbm-dev libgles2-mesa-dev
```

You will also need to connect your Raspberry Pi to a screen. The boot config from `/boot/config.txt` that I have used for my tests, if it helps in any way:

```bash
dtoverlay=vc4-fkms-v3d
max_framebuffers=2
hdmi_force_hotplug=1
hdmi_group=2
hdmi_mode=81
```

**How do I try it?**

Copy or download the `margin.c` file onto your Raspberry Pi. Using terminal, write the following commands to compile the source file:

```
gcc -o margin margin.c -ldrm -lgbm -lEGL -lGLESv2 -I/usr/include/libdrm -I/usr/include/GLES2
```

To run the executable, type the following:

```
margin 100 # all the same
margin 50 100 #top/bottom and left/right the same
margin 20 10 15 15 # all custom, 
```

## Troubleshooting and Questions

**Failed to get EGL version! Error:**

Your EGL might be faulty! Make sure you are using the libraries provided by Raspbian and not the ones installed through apt-get or other package managers. Use the ones in the `/opt/vc/lib` folder. If that does not work, try reinstalling your Raspberry Pi OS.

**I get "Error! The glViewport/glGetIntegerv are not working! EGL might be faulty!" What should I do?**

Same as above.

## License

Do whatever you want.

```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
```
