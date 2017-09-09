#include <csignal>
#include <utils/Logger.h>
#include <Constants.h>
#include "core/Server.h"

#define SIGTERM 15
#define SIGINT  2

void term(int sig) {
    uGame::Server::instance()->stop();
}

int main() {
    uGame::Logger::init(APP_DIR);
    std::signal(SIGTERM, term);
    std::signal(SIGINT, term);
    uGame::Server::instance()->run();
    uGame::Logger::clean();
    delete uGame::Server::instance();
    return 0;
}