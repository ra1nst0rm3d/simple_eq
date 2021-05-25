#!/bin/bash

STR=$(pactl list sinks | grep null)

echo $STR
