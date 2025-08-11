cmake_minimum_required(VERSION 3.10)
project(led-control-service)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(led-service src/service.cpp src/gpio_manager.cpp src/parser.cpp)
target_link_libraries(led-service gpiod pthread)

add_executable(ledctl src/client.cpp)
target_link_libraries(ledctl pthread)

install(TARGETS led-service ledctl DESTINATION /usr/bin)
install(FILES config/leds.ini DESTINATION /etc/led-control)
install(FILES systemd/led-control.service DESTINATION /lib/systemd/system)
