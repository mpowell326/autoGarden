# Process to upload a program onto the Stellaris

target remote | openocd -c "gdb_port pipe; log_output openocd.log" -f ~/src/ence463-starter-gcc/LM3S1968.cfg
#target remote tcp:localhost:3333
monitor reset halt
load
#continue
monitor reset
