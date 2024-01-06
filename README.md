Discontinued in favor of the fantastic [xremap](https://github.com/k0kubun/xremap) key mapper for both Wayland and X11.
Go take a look at `xremap`, you will not regret it if TouchCursor was what you were interested in.

# touchcursor-linux
This application was designed to remap the `uiophjklnmy` keys to different movement keys when the spacebar is pressed down, allowing you to keep your hands on the home row. It has grown to allow remapping all the keys, even the hyper key.

```
i - up
j - left
k - down
l - right
u - home
o - end
p - backspace
h - page up
n - page down
m - del
y - insert
```

# Removing previous installation
1. Disable the old service
`sudo systemctl stop touchcusor.service`
`sudo systemctl disable touchcusor.service`
2. Remove the old service file
`sudo rm /etc/systemd/system/touchcursor.service`
3. Remove the old configuration file
`sudo rm -r /etc/touchcursor`

# How to install
1. Clone or download this repo.
2. Open terminal in the downloaded directory.
3. `cmake .` to create custom Makefile.
4. `make` to build the project: application and (if Catch2 is present) tests.
5. `make deploy` to install the application.
6. Modify the config file (`~/.config/touchcursor/touchcursor.conf`) to your liking.
7. Restart the service `systemctl --user restart touchcursor.service`.

# Tests
You can run tests after building the project if Catch2 version 3+ is present in the system with:
```
./out/tests
```

# Thanks to
[Thomas Bocek, Dvorak](https://github.com/tbocek/dvorak): Check him out and thanks for the starting point. Good examples for capturing and modifying keyboard input in Linux, specifically Wayland.

[Martin Stone, Touch Cursor](https://github.com/martin-stone/touchcursor): Wonderful project for cursor movement when coding.

# Special note
This application works under Xorg and **Wayland**.
