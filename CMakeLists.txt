cmake_minimum_required(VERSION 3.10)
add_compile_options(-Wall -Wextra -Werror -Wpedantic)
project(yubikey-hack)
add_executable(yubikey-hack src/get_token.c)
install(TARGETS yubikey-hack DESTINATION /usr/bin)
