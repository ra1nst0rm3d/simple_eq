# SimpleEQ
Simple parametric EQ configurable by config file. Needs PulseAudio for thread management

# What ready to use now?
- Filter class (it's pretty fast)

# What is pretty buggy? :)
- Other non-PA platforms: Win., Mac OS
# What I need to know for build?
- Library: RTAudio, PulseAudio
# How to build?
` g++ (or clang++) -ffast-math -lrtaudio src/Daemon_Head.cc src/Filter.cc `
# How to launch it?
Set input of EQ to monitor of null sink, output to output device -> Output all your apps to null sink -> ??? Profit!
