# YubiKey hack

An experimental remote YubiKey token retrieval service using a Linux host, an
ESP32-controlled relay, and a YubiKey. A TCP request triggers the relay, reads the
resulting token, and returns it to the client.

## Описание

Экспериментальный сервис для удалённого получения токена YubiKey с Linux-хоста через реле
под управлением ESP32. TCP-запрос включает реле, считывает полученный токен и возвращает его
клиенту.

Программа на Linux отправляет команду ESP32 через последовательный порт. Контроллер переключает
реле, после чего программа считывает токен YubiKey из устройства ввода Linux и возвращает его
TCP-клиенту.

Репозиторий сохранен как узкий аппаратно-программный эксперимент, а не как универсальный продукт для удаленной аутентификации. Он предполагает доверенную среду эксплуатации и прямой контроль над подключенным оборудованием.

## Состав

```text
src/get_token.c
esp32/get_token_esp32.ino
```

`get_token.c` - Linux service. `get_token_esp32.ino` - минимальная прошивка ESP32, управляющая relay input.

## Сборка Linux части

```sh
cmake --preset release
cmake --build --preset release
```

Исполняемый файл:

```text
build/release/yubikey-hack
```

## Настройка

Сервис берет адрес и порт из переменных окружения:

```sh
IP=0.0.0.0 \
PORT=12345 \
SERIAL_DEV=/dev/ttyUSB0 \
USB_DEV=/dev/input/event0 \
PASS=prefix \
./build/release/yubikey-hack
```

Переменные:

- `IP` и `PORT` - адрес TCP service;
- `SERIAL_DEV` - serial device ESP32;
- `USB_DEV` - Linux input device, через которое читается ввод YubiKey;
- `PASS` - строка-префикс, добавляемая перед считанным token.

Для работы сервису нужен доступ к указанным serial/input devices.

## ESP32

Скетч `esp32/get_token_esp32.ino` слушает serial-команду `k` и кратковременно переключает relay pin. По умолчанию используется `RELAY_IN = 5`.

## Статья

Описание исходной идеи и аппаратной схемы: [статья на Habr](https://habr.com/ru/articles/825858/).
