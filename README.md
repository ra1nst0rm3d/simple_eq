# SimpleEQ
Simple parametric EQ configurable by config file. Needs PulseAudio for thread management

# What ready to use now?
- Filter class (it's pretty fast)

# What is pretty buggy? :)
- Other non-PA platforms: Win., Mac OS
# What I need to know for build?
- Library: RTAudio, PulseAudio
# How to build?
` g++ (or clang++) -lrtaudio src/Daemon_Head.cc src/Filter.cc `
