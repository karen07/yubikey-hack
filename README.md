# YubiKey hack

YubiKey hack is a small remote token retrieval service built around a YubiKey, an ESP32-controlled relay, and a Linux host.

The Linux program exposes a TCP service. When a client connects, it sends a command over the serial link to the ESP32, which toggles the relay; the program then reads the token typed by the YubiKey from a Linux input device and returns that token to the TCP client.

The repository is preserved as a focused hardware/software experiment rather than a general remote-authentication product. It assumes trusted deployment conditions and direct control over the attached hardware.

## Описание

YubiKey hack - небольшой сервис удаленного получения токена, построенный вокруг YubiKey, реле под управлением ESP32 и Linux хоста.

Программа Linux поднимает TCP сервис. При подключении клиента она отправляет команду ESP32 по последовательному соединению, ESP32 переключает реле, после чего программа считывает токен, введенный YubiKey, через устройство ввода Linux и возвращает этот токен TCP клиенту.

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
