#!/bin/bash
git clone https://github.com/curl/curl.git
git checkout 6a45abfbf6c5e366b05524301e1dfeacf348be26
cd curl
autoreconf -fi
./configure --with-openssl --enable-versioned-symbols
make
sudo make install

#if you have problem with libcurl.so.4 - execute in cmd line
#sudo rm /usr/local/lib/libcurl.so.4
#sudo ln -s /usr/lib/x86_64-linux-gnu/libcurl.so.4.7.0 /usr/local/lib/libcurl.so.4

