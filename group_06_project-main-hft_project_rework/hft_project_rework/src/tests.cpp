#include "application.h"
#include "tcp.h"
#include "ouch.h"

#include <arpa/inet.h>
#include <map>
#include <thread>
#include <vector>
#include <cassert>
#include <csignal>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <string.h>
#include <iostream>
#include <sstream>

// RawSocket *sock = NULL;

void start_app() {
    Application app;
    app.test();
}

void init_test(int& fd, order_message& message, RawSocket*& sock) {
    sock = new RawSocket();
    sock->init();
    fd = sock->open();
    struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(12345);
    inet_pton(AF_INET, "10.0.2.15", &saddr.sin_addr);
    sock->connect(fd, saddr);
    message.type = 'O';
    memcpy(&message.order_token, "ABCDEFGHIJKLM", 14);
    memcpy(&message.firm, "UIUC", 4);
}

// 1 buy test
void test1() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    
    message.indicator = 'B';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    if (buffer[0] == 'A') {
        std::cout << "Test 1: Client sent 1 buy order and system acknowledged!\n";
    }
    else {
        throw std::runtime_error("Test 1 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// multi buy test
void test2() {
    RawSocket *sock = NULL;
    // std::cout << "1\n";
    std::thread t (start_app);
    // std::cout << "2\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    
    int check = 0;
    
    message.indicator = 'B';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    
    for (int i = 0; i < 3; i++) {
        
        memset(buffer,0,100);
        sock->send(fd, (uint8_t *)&message, sizeof(message));    
        // std::cout << (3+i) << "\n";
        sock->recieve(fd, buffer, sizeof(accept_order_message));
        check += (buffer[0] == 'A') ? 1 : 0;
    }
    if (check == 3) {
        std::cout << "Test 2: Client sent 3 buy orders and system acknowledged!\n";
    }
    else {
        throw std::runtime_error("Test 2 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 1 sell test
void test3() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    
    message.indicator = 'S';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    if (buffer[0] == 'A') {
        std::cout << "Test 3: Client sent 1 sell order and system acknowledged!\n";
    }
    else {
        throw std::runtime_error("Test 3 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// multi sell test
void test4() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'S';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    for (int i = 0; i < 3; i++) {
        memset(buffer,0,100);
        sock->send(fd, (uint8_t *)&message, sizeof(message));    
        sock->recieve(fd, buffer, sizeof(accept_order_message));
        check += (buffer[0] == 'A') ? 1 : 0;
    }
    if (check == 3) {
        std::cout << "Test 4: Client sent 3 sell orders and system acknowledged!\n";
    }
    else {
        throw std::runtime_error("Test 4 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 1 buy 1 sell test, same amount and price
void test5() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'B';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'S';
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->recieve(fd, buffer, sizeof(executed_order_message));
    check += (buffer[0] == 'E') ? 1 : 0;

    if (check == 3) {
        std::cout << "Test 5: Client sent 1 buy and 1 sell order at same price and shares and system executed the order!\n";
    }
    else {
        throw std::runtime_error("Test 5 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 1 sell 1 buy test, same amount and price
void test6() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'S';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'B';
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->recieve(fd, buffer, sizeof(executed_order_message));
    check += (buffer[0] == 'E') ? 1 : 0;

    if (check == 3) {
        std::cout << "Test 6: Client sent 1 sell and 1 buy order at same price and shares and system executed the order!\n";
    }
    else {
        throw std::runtime_error("Test 6 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 2 buy 1 sell test, total shares same, same price
void test7() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'B';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'S';
    message.shares = 200;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->recieve(fd, buffer, sizeof(executed_order_message));
    check += (buffer[0] == 'E') ? 1 : 0;

    if (check == 4) {
        std::cout << "Test 7: Client sent 2 buy and 1 sell order at same price and same total shares and system executed the order!\n";
    }
    else {
        throw std::runtime_error("Test 7 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 2 sell 1 buy test, total shares same, same price
void test8() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'S';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'B';
    message.shares = 200;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->recieve(fd, buffer, sizeof(executed_order_message));
    check += (buffer[0] == 'E') ? 1 : 0;

    if (check == 4) {
        std::cout << "Test 8: Client sent 2 sell and 1 buy order at same price and same total shares and system executed the order!\n";
    }
    else {
        throw std::runtime_error("Test 8 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 1 buy 1 sell test, total shares different, same price
void test9() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'B';
    message.shares = 200;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    // sock->send(fd, (uint8_t *)&message, sizeof(message));    
    // sock->recieve(fd, buffer, sizeof(accept_order_message));
    // check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'S';
    message.shares = 150;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->recieve(fd, buffer, sizeof(executed_order_message));
    executed_order_message* recv_m = (executed_order_message*) buffer; 
    // std::cout << recv_m->shares << "\n";
    // sock->recieve(fd, buffer, sizeof(executed_order_message));
    // recv_m = (executed_order_message*) buffer; 
    // std::cout << recv_m->shares << "\n";
    if (recv_m->shares == 150) {check++;}
    // check += (buffer[0] == 'E') ? 1 : 0;

    if (check == 3) {
        std::cout << "Test 9: Client sent 1 buy and 1 sell order at same price but different total shares and system executed the order for lower number of shares!\n";
    }
    else {
        throw std::runtime_error("Test 9 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 1 sell 1 buy test, total shares different, same price
void test10() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'S';
    message.shares = 200;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    // sock->send(fd, (uint8_t *)&message, sizeof(message));    
    // sock->recieve(fd, buffer, sizeof(accept_order_message));
    // check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'B';
    message.shares = 150;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->recieve(fd, buffer, sizeof(executed_order_message));
    executed_order_message* recv_m = (executed_order_message*) buffer; 
    if (recv_m->shares == 150) {check++;}
//    check += (buffer[0] == 'E') ? 1 : 0;

    if (check == 3) {
        std::cout << "Test 10: Client sent 1 sell and 1 buy order at same price but different total shares and system executed the order for lower number of shares!\n";
    }
    else {
        throw std::runtime_error("Test 10 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

// 1 buy 1 sell test, total shares same, different price
// buying price higher than selling price
void test11() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    int check = 0;
    
    message.indicator = 'B';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'S';
    message.price = 90;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    sock->recieve(fd, buffer, sizeof(accept_order_message));
    check += (buffer[0] == 'A') ? 1 : 0;

    sock->recieve(fd, buffer, sizeof(executed_order_message));
    executed_order_message* recv_m = (executed_order_message*) buffer; 
    //if (recv_m->shares == 150) {check++;}
    if (recv_m->execution_price == 90 && buffer[0] == 'E') {check++;}
    // check += (buffer[0] == 'E') ? 1 : 0;

    if (check == 3) {
        std::cout << "Test 11: Client sent 1 buy and 1 sell order at different price but same total shares and system executed the order as bid was higher than ask!\n";
    }
    else {
        throw std::runtime_error("Test 11 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
}

int test12_count = 0;

void receiving_msgs(int fd, RawSocket* sock) {
    uint8_t buffer[100];
    while(1) {
        memset(buffer, 0, 100);
        sock->recieve(fd, buffer, 100);
        if (buffer[0] == 'A') {
            test12_count++;
            // std::cout << "A\n";
        }
        else if (buffer[0] == 'J') {
            std::cout << "Order Rejected!\n";
        }
        else if (buffer[0] == 'E') {
            test12_count++;
            // std::cout << "E\n";
        }
    }
}

// 1 buy 1 sell test, total shares same, different price
// buying price lower than selling price
void test12() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    std::thread t1 (receiving_msgs, fd, sock);
    
    message.indicator = 'B';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 60;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    // sock->recieve(fd, buffer, sizeof(accept_order_message));
    // check += (buffer[0] == 'A') ? 1 : 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    message.indicator = 'S';
    message.price = 90;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    // sock->recieve(fd, buffer, sizeof(accept_order_message));
    // check += (buffer[0] == 'A') ? 1 : 0;

    // sock->recieve(fd, buffer, sizeof(executed_order_message));
    // executed_order_message* recv_m = (executed_order_message*) buffer; 
    //if (recv_m->shares == 150) {check++;}
    // if (recv_m->execution_price == 90 && buffer[0] == 'E') {check++;}
    // check += (buffer[0] == 'E') ? 1 : 0;

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    if (test12_count == 2) {
        std::cout << "Test 12: Client sent 1 buy and 1 sell order at different price but same total shares and system did not execute the order as bid was lower than ask!\n";
    }
    else {
        throw std::runtime_error("Test 12 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
    t1.detach();
}

int test13_num_shares = 0;

void receiving_msgs_t13(int fd, RawSocket* sock) {
    uint8_t buffer[100];
    while(1) {
        memset(buffer, 0, 100);
        sock->recieve(fd, buffer, 100);
        if (buffer[0] == 'A') {
            // test12_count++;
            std::cout << "A\n";
        }
        else if (buffer[0] == 'J') {
            std::cout << "Order Rejected!\n";
        }
        else if (buffer[0] == 'E') {
            // test12_count++;
            std::cout << "E\n";
            executed_order_message* recv_m = (executed_order_message*) buffer;
            test13_num_shares += recv_m->shares;
        }
        else {
            std::cout << "else\n";
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 1 buy 1 sell test, total shares different, same price
void test13() {
    RawSocket *sock = NULL;
    std::thread t (start_app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int fd;// = sock->open();
    uint8_t buffer[100];
    order_message message;
    init_test(fd, message, sock);
    // int check = 0;
    std::thread t1 (receiving_msgs_t13, fd, sock);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    message.indicator = 'B';
    message.shares = 100;
    memcpy(&message.stock, "AAPL", 5);
    message.price = 100;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    // sock->recieve(fd, buffer, sizeof(accept_order_message));
    // check += (buffer[0] == 'A') ? 1 : 0;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // sock->send(fd, (uint8_t *)&message, sizeof(message));    
    // sock->recieve(fd, buffer, sizeof(accept_order_message));
    // check += (buffer[0] == 'A') ? 1 : 0;

    message.indicator = 'S';
    message.shares = 150;
    sock->send(fd, (uint8_t *)&message, sizeof(message));    
    // sock->recieve(fd, buffer, sizeof(accept_order_message));
    // check += (buffer[0] == 'A') ? 1 : 0;

    // sock->recieve(fd, buffer, sizeof(executed_order_message));
    // executed_order_message* recv_m = (executed_order_message*) buffer; 
    // std::cout << recv_m->shares << "\n";
    // sock->recieve(fd, buffer, sizeof(executed_order_message));
    // recv_m = (executed_order_message*) buffer; 
    // std::cout << recv_m->shares << "\n";
    // if (recv_m->shares == 150) {check++;}
    // check += (buffer[0] == 'E') ? 1 : 0;

    std::cout << test13_num_shares << "\n";
    if (test13_num_shares == 100) {
        std::cout << "Test 13: Client sent 1 buy and 1 sell order at same price but different total shares and system executed the order for lower number of shares!\n";
    }
    else {
        throw std::runtime_error("Test 13 failed!\n");
    }
    
    memset(&message, 0, sizeof(order_message));
    message.indicator = '1';
    sock->send(fd, (uint8_t *)&message, sizeof(message));
    sock->close(fd);
    t.join();
    t1.detach();
}

int main(int argc, char** argv) {
    std::stringstream ss(argv[1]);
    int sw = 0;
    ss >> sw;
    switch(sw) {
        case 1:
            test1();
            break;
        case 2:
            test2();
            break;
        case 3:
            test3();
            break;
        case 4:
            test4();
            break;
        case 5:
            test5();
            break;
        case 6:
            test6();
            break;
        case 7:
            test7();
            break;
        case 8:
            test8();
            break;
        case 9:
            test9();
            break;
        case 10:
            test10();
            break;
        case 11:
            test11();
            break;  
        case 12:
            test12();
            break;
        case 13:
            test13();
            break;      
    }
    // test1();
    // std::cout << "0\n";
    // test2();
    // test3();
    // test4();
    // test5();
    // test6();
    // test7();
    // test8();
    // test9();
    // test10();
    // test11();
}