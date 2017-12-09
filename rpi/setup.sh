# For Raspberry Pi 3

# Start pigpio as a daemon, at a sample rate of 2 microseconds
sudo pigpiod -s 2
# Get a notification handle
pigs no
# Select gpios 2 and 3
pigs nb 0 0xC
# Start notifications for SCL/SDA
./bin/pig2i2c 3  2 </dev/pigpio0