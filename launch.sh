#!/bin/bash
STR='pactl list sinks | grep null'
if [ -z "$STR" ]; then
	pactl load-module module-null-sink
fi
killall SimpleEQ 2&>1 
./SimpleEQ $* &
