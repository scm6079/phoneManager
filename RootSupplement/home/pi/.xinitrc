#!/bin/sh
while true; do
	# Disable DPMS / Screen blanking
	xset -dpms
	xset s off

	# Hide the cursor (move it to the bottom-right, comment out if you want mouse interaction)
	xwit -root -warp $( cat /sys/module/*fb*/parameters/fbwidth ) $( cat /sys/module/*fb*/parameters/fbheight )

	# Start the window manager (remove "-use_cursor no" if you actually want mouse interaction)
	matchbox-window-manager -use_titlebar no -use_cursor no &

	/home/pi/src/phoneManager/phoneManager

	sleep 2;
done;

