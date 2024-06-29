#include "../include/WebServer.hpp"

int main(int argc, char *argv[]) {
    if (argc > 2)
    {
        std::cerr << "You cannot enter multiple configuration files.\n";
        return 1;
    }

    WebServer webServer;

    if (argc == 2)
        webServer.init(argv[1]);
    else
    {
        std::string defaultFile = "default_files/default.conf";
        webServer.init(defaultFile.c_str());
    }
    
    webServer.start();

    return 0;
}