#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

void serial_send_command(void)
{
    char *serial_dev = getenv("SERIAL_DEV");
    const char check_sym = 'k';

    int serial_port = open(serial_dev, O_RDWR);
    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    unsigned char msg[] = { check_sym, '\r' };
    int n = 0;
    n = write(serial_port, msg, sizeof(msg));
    if (n != sizeof(msg)) {
        printf("Wrong write data\n");
        exit(EXIT_FAILURE);
    }

    char read_buf[256];
    memset(&read_buf, '\0', sizeof(read_buf));

    int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

    if (num_bytes < 0) {
        printf("Error reading: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (read_buf[0] != check_sym) {
        printf("Wrong read data\n");
        printf("Read %i bytes. Received message: %s", num_bytes, read_buf);
        exit(EXIT_FAILURE);
    }

    close(serial_port);
}

void usb_read_token(char *token)
{
    char *usb_dev = getenv("USB_DEV");

    struct pollfd fds[1];
    fds[0].fd = open(usb_dev, O_RDONLY | O_NONBLOCK);
    fds[0].events = POLLIN;

    if (fds[0].fd < 0) {
        printf("Error unable open for reading '%s'\n", usb_dev);
        exit(EXIT_FAILURE);
    }

    char keys[100] = { 0,  1,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 12,
                       13, 14, 15,  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
                       26, 27, 28,  29,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
                       39, 40, 41,  42,  43,  'z', 'x', 'c', 'v', 'b', 'n', 'm' };

    struct input_event ev;
    int char_count = 0;

    char *pass = getenv("PASS");
    sprintf(token, "%s", pass);

    while (true) {
        int timeout_ms = 1000;
        int ret = poll(fds, 1, timeout_ms);

        if (ret > 0) {
            if (fds[0].revents) {
                ssize_t r = read(fds[0].fd, &ev, sizeof(ev));

                if (r < 0) {
                    printf("Error %d\n", (int)r);
                    break;
                } else {
                    if (ev.type == 1 && ev.value == 1) {
                        sprintf(token + strlen(token), "%c", keys[ev.code]);
                        char_count++;
                        if (char_count >= 44) {
                            sprintf(token + strlen(token), "\n");
                            break;
                        }
                    }
                }
            }
        }
    }

    close(fds[0].fd);
}

int main(void)
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    char token[100];

    uint32_t cli_len = sizeof(cli);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Socket successfully created..\n");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(getenv("IP"));
    servaddr.sin_port = htons(atoi(getenv("PORT")));

    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Socket successfully binded..\n");
    }

    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Server listening..\n");
    }

    while (1) {
        connfd = accept(sockfd, (struct sockaddr *)&cli, &cli_len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(EXIT_FAILURE);
        } else {
            printf("server accept the client...\n");
        }

        serial_send_command();

        usb_read_token(token);

        size_t n = 0;
        n = write(connfd, token, strlen(token));
        if (n != strlen(token)) {
            printf("Wrong write data\n");
            exit(EXIT_FAILURE);
        }

        close(connfd);

        fflush(stdout);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
