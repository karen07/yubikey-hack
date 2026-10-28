# YubiKey Hack

An experimental remote YubiKey token retrieval service using a Linux host, an
ESP32-controlled relay, and a YubiKey. A TCP request triggers the relay, reads the
resulting token, and returns it to the client.

Экспериментальный сервис для удалённого получения токена YubiKey с Linux-хоста через реле
под управлением ESP32. TCP-запрос включает реле, считывает полученный токен и возвращает его
клиенту.

## Описание

Программа на Linux отправляет команду ESP32 через последовательный порт. Контроллер переключает
реле, после чего программа считывает токен YubiKey из устройства ввода Linux и возвращает его
TCP-клиенту.

Репозиторий сохранен как узкий аппаратно-программный эксперимент, а не как универсальный продукт для удаленной аутентификации. Он предполагает доверенную среду эксплуатации и прямой контроль над подключенным оборудованием.

## Разработка и проверка изменений

**Основные точки изменения:**

- `src/get_token.c` - Linux-программа: TCP-запрос, последовательный порт и чтение токена;
- `esp32/get_token_esp32.ino` - управление реле со стороны ESP32;
- `CMakeLists.txt` - сборка Linux-программы.

**Проверка изменений:** Изменение обмена с ESP32 требует согласованной правки кода Linux и прошивки контроллера. Проверяйте последовательность срабатывания реле и работу с устройством отдельно от сетевого интерфейса; не записывайте реальные токены в тестовые данные и логи.

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

## Лицензия

Проект распространяется под лицензией GNU Affero General Public License v3.0. Подробности см. в файле [LICENSE](LICENSE).
