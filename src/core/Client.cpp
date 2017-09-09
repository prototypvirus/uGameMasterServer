//
// Created by symbx on 09.09.17.
//

#include <SFML/Network/Packet.hpp>
#include <core/Query.h>
#include <core/Server.h>
#include <utils/MD5.h>
#include <random>
#include <climits>
#include "core/Client.h"

uGame::Client::Client(sf::TcpSocket *sock) {
    _sock = sock;
    _kick = false;
}

uGame::Client::~Client() {
    delete _sock;
}

sf::TcpSocket *uGame::Client::socket() {
    return _sock;
}

void uGame::Client::receive() {
    sf::Packet packet;
    if(_sock->receive(packet) != sf::Socket::Done)
        return;
    sf::Uint16 id = 0;
    if(!(packet >> id)) {
        bad();
        return;
    }
    switch (id) {
        case HandShake: {
            sf::Packet resp;
            if (Server::instance()->isMaintenance()) {
                resp << Maintenance;
                _sock->send(resp);
                return;
            }
            std::string version;
            if (!(packet >> version)) {
                bad();
                return;
            }
            if (version != Server::instance()->getVersion()) {
                resp << UpdateRequire;
                resp << Server::instance()->getVersion();
                _sock->send(resp);
                return;
            }
            resp << Continue;
            _sock->send(resp);
            return;
        }

        case Authorize: {
            std::string username;
            std::string password;
            sf::Packet resp;
            if (!(packet >> username >> password)) {
                bad();
                return;
            }
            Query *q = Server::instance()->query(
                    "SELECT `id`,`password`,`ban` FROM `users` WHERE `email` == '" + Server::escape(username) + "';");
            if(q == NULL) {
                bad();
                return;
            }
            if(q->getRowsCount() < 1) {
                delete q;
                resp << IncorrectCredentials;
                _sock->send(resp);
                return;
            }
            delete q;
            MYSQL_ROW row = q->getRow();
            std::string sid = row[0];
            std::string pass = row[1];
            std::string sban = row[2];
            if(MD5(std::string(password.rbegin(), password.rend())).hexdigest() != pass) {
                resp << IncorrectCredentials;
                _sock->send(resp);
                return;
            }
            if(std::stoi(sban) > 0) {
                resp << Banned;
                resp << (sf::Uint8)std::stoi(sban);
                _sock->send(resp);
                return;
            }
            sf::Uint64 session[] = {
                getRandom(),
                getRandom()
            };
            q = Server::instance()->query("INSERT INTO `session` VALUES (NULL, NOW(), '"+sid+"', "+std::to_string(session[0])+", "+std::to_string(session[0])+");");
            if(q != NULL)
                delete q;
            sf::Uint8 rev = static_cast<sf::Uint8>(rand() % 2);
            resp << ResultInfo;
            resp << rev;
            if(rev == 0)
                resp << (sf::Uint64)session[0] << (sf::Uint64)std::stoll(sid) << (sf::Uint64)session[1];
            else
                resp << (sf::Uint64)session[1] << (sf::Uint64)std::stoll(sid) << (sf::Uint64)session[0];
            _sock->send(resp);
            return;
        }

        default:
            bad();
            return;
    }
}

bool uGame::Client::toKick() {
    return _kick;
}

void uGame::Client::bad() {
    _kick = true;
}

unsigned long uGame::Client::getRandom() {
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> gen(0, ULONG_MAX);
    return gen(rng);
}
