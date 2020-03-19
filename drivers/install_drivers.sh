#!/bin/sh

sudo apt --fix-broken install libsqlcipher0 ./deps/libuv0.10_0.10.22-2_amd64.deb libpthread-stubs0-dev build-essential

# Targeted Linux distribution is Ubuntu Linux 16.04 LTS
# Tested on  Ubuntu 18.10 x86_64 with Cinnamon. Kernel  4.18.0-16-generic

# Install Tobii USB Host (mandatory) that handles connections to the tracker:
sudo dpkg -i tobiiusbservice_l64U14_2.1.5-28fd4a.deb

# Install Tobii Engine daemon (recommended) that offers extended functionality
sudo dpkg -i tobii_engine_linux-0.1.6.193_rc-Linux.deb

sudo cp ./tobii.conf /etc/ld.so.conf.d/
