# libhs - public domain
# Niels Martignène <niels.martignene@protonmail.com>
# https://neodd.com/libraries

# This software is in the public domain. Where that dedication is not
# recognized, you are granted a perpetual, irrevocable license to copy,
# distribute, and modify this file as you see fit.

# See the LICENSE file for more details.

add_executable(monitor_devices monitor_devices.c)
target_link_libraries(monitor_devices libhs)
