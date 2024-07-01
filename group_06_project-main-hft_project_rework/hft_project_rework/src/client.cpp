#include <cassert>
#include <csignal>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <string.h>
#include <iostream>
#include <sstream>

#include "tcp.h"
#include "ouch.h"

RawSocket *sock = NULL;

void receiving_msgs(int fd) {
    uint8_t buffer[100];
    while(1) {
        memset(buffer, 0, 100);
        sock->recieve(fd, buffer, 100);
        if (buffer[0] == 'A') {
            accept_order_message* buf = (accept_order_message*) buffer;
            std::cout << "Order Accepted! " << buf->order_token << "\n";
            std::cout << ((buf->indicator == 'B') ? "bought " : "sold ") << " ";
            std::cout << buf->shares << " shares of " << buf->stock << " at price" << buf->price << "\n";
        }
        else if (buffer[0] == 'J') {
            std::cout << "Order Rejected!\n";
        }
        else if (buffer[0] == 'E') {
            executed_order_message* msg = (executed_order_message*) buffer;
            std::cout << "Order Executed! " << msg->order_token << "\n";
            std::cout << msg->shares << " shares bought at price ";
            std::cout << msg->execution_price << "\n";
        }
    }
}

int main(int argc, char** argv) {
    sock = new RawSocket();
    sock->init();
    int fd = sock->open();
    struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(12345);
    inet_pton(AF_INET, "10.0.2.15", &saddr.sin_addr);
    sock->connect(fd, saddr);
    order_message message;
    message.type = 'O';
    memcpy(&message.order_token, "ABCDEFGHIJKLM", 14);
    memcpy(&message.firm, argv[1], 4);
    std::thread recv_t(receiving_msgs, fd);
    recv_t.detach();

    while (1) {
        std::string in;
        bool flag = false;
        while (1) {
            std::cout << "buy or sell or exit?" << std::endl;
            std::cin >> in;
            if (in == "exit") {flag = true; break;}
            if (in != "buy" && in != "sell") {
                std::cout << "invalid" << std::endl;
                continue;
            }
            message.indicator = (in == "buy") ? 'B' : 'S';
            break;
        }
        // message.indicator = 'B';

        if (flag) {break;}
        
        std::cout << "shares?" << std::endl;
        std::cin >> in;
        std::stringstream ss(in);
        ss >> message.shares;
        // message.shares = 100;

        memcpy(&message.stock, "AAPL", 5);

        std::cout << "price?" << std::endl;
        std::cin >> in;
        std::stringstream ss1(in);
        ss1 >> message.price;
        // message.price = 500;
        
        
        sock->send(fd, (uint8_t *)&message, sizeof(message));
        
        // sock->recieve(fd, buffer, sizeof(accept_order_message));
        // std::cout << buffer[0] << std::endl;
        // message.indicator = 'S';
        // sock->send(fd, (uint8_t *)&message, sizeof(message));
        // sock->recieve(fd, buffer, sizeof(accept_order_message));
        // std::cout << buffer[0] << std::endl;
        // if (argv[1] == "VALK") {
        //     sock->recieve(fd, buffer, sizeof(executed_order_message));
        //     std::cout << buffer[0] << std::endl;
        // }
    }

    // for (int i = 0; i < 2; i++) {
    //     sock->recieve(fd, buffer, sizeof(executed_order_message));
    //     std::cout << buffer[0] << std::endl;
    // }
    sock->close(fd);
    return 0;
}