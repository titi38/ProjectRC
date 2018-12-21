#!/bin/bash
clear
raspivid -n -t 0 -w 1280 -h 720 -fps 25 -b 6000000 -o - | gst-launch-1.0 -e -vvvv fdsrc ! h264parse ! rtph264pay pt=96 config-interval=5 ! udpsink host=134.158.40.214 port=5000
