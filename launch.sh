#!/bin/bash
STR=$(pactl list sinks | grep null)
if [ -z "$STR" ]; then
	pactl load-module module-null-sink
fi
killall SimpleEQ > /dev/null
./SimpleEQ $* &
