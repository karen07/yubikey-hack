cmake_minimum_required(VERSION 2.6)

project(yubikey-hack)

add_executable(yubikey-hack get_token.c)

install(TARGETS yubikey-hack RUNTIME DESTINATION /usr/bin/)
