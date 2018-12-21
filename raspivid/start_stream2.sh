#!/bin/bash
clear
raspivid -n -t 0 -cd MJPEG -w 1280 -h 720 -fps 30 -b 800000000 -o - | gst-launch-1.0 -e -vvvv fdsrc ! "image/jpeg,framerate=30/1" ! jpegparse ! rtpjpegpay ! udpsink host=134.158.40.214 port=5000
