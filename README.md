# SimpleEQ
Simple parametric EQ configurable by config file. Needs PulseAudio for thread management

# What ready to use now?
- Filter class (it's pretty fast)

# What is pretty buggy? :)
- Other non-PW platforms: Win., Mac OS
# What I need to know for build?
- Library: RTAudio, PipeWire (PulseAudio very buggy)
# How to build?
` make (for release build)`
` make debug (for debug build)`
# How to launch it?
Set input of EQ to monitor of null sink, output to output device -> Output all your apps to null sink -> ??? Profit!
