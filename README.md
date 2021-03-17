# SimpleEQ
Simple parametric EQ configurable by config file. Needs PulseAudio for thread management

# What ready to use now?
- Filter class (it's pretty fast)

# What is pretty buggy? :)
- Audio processing (latency increasing over time)
- Other non-PA platforms: Win., Mac OS
# What I need to know for build?
- Library: RTAudio, PulseAudio
- STD: C++20
- 
# How to build?
` g++ (or clang++) -lrtaudio -std=c++20 src/Daemon_Head.cc src/Filter.cc `
