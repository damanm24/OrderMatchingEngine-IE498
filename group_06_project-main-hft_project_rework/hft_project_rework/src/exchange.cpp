#include "application.h"

#include <arpa/inet.h>
#include <map>
#include <thread>
#include <vector>


int main() {
    Application app;
    if (!TEST) {
        std::cout << "FIRM" << "\t";
        std::cout << "STOCK" << "\t";
        std::cout << "TYPE" << "\t";
        std::cout << "SHARES" << "\t";
        std::cout << "PRICE" << "\t";
        std::cout << "TIME" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
    }
    app.start();
    return 0;
}