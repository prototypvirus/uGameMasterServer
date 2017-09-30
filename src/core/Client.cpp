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

namespace uGame {

    Client::Client(sf::TcpSocket *sock) {
        _sock = sock;
        _state = StateNone;
    }

    Client::~Client() {
        delete _sock;
    }

    sf::TcpSocket *Client::socket() {
        return _sock;
    }

    void Client::receive() {
        sf::Packet packet;
        if (_sock->receive(packet) != sf::Socket::Done)
            return;
        sf::Uint16 id = 0;
        if (!(packet >> id))
            return bad();

        switch (id) {
            case RequestHandShake:
                return handshake(packet);

            case RequestAuthorize:
                return authorize(packet);

            default:
                bad();
                return;
        }
    }

    bool Client::toKick() {
        return (_state & StateKick) != 0;
    }

    void Client::bad() {
        _state |= StateKick;
    }

    unsigned long Client::getRandom() {
        std::mt19937 rng;
        rng.seed(std::random_device()());
        std::uniform_int_distribution<std::mt19937::result_type> gen(0, ULONG_MAX);
        return gen(rng);
    }

    void Client::handshake(sf::Packet in) {
        if((_state & RequestHandShake) != 0)
            return bad();

        sf::Packet resp;
        if (Server::instance()->isMaintenance()) {
            resp << ResponseMaintenance;
            _sock->send(resp);
            return;
        }
        std::string version;
        if (!(in >> version))
            return bad();
        if (version != Server::instance()->getVersion()) {
            resp << ResponseUpdateRequire;
            resp << Server::instance()->getVersion();
            _sock->send(resp);
            return;
        }
        std::string hwid;
        if(!(in >> hwid))
            return bad();

        Query *q = Server::instance()->query(
                "SELECT `state` FROM `hardware` WHERE `hash` == '" + Server::escape(hwid) + "';");
        if (q == NULL)
            return bad();

        if(q->getRowsCount() > 0) {
            sf::Uint8 hwState = static_cast<sf::Uint8>(std::stoi(q->getRow()[0]));
            resp << hwState;
        }else
            resp << (sf::Uint8)0;
        resp << ResponseContinue;
        _sock->send(resp);
        _state |= StateHandshake;
    }

    void Client::authorize(sf::Packet in) {
        if((_state & RequestHandShake) == 0 || (_state & StateAuthorized) != 0)
            return bad();

        std::string username;
        std::string password;
        sf::Packet resp;
        if (!(in >> username >> password))
            return bad();

        Query *q = Server::instance()->query(
                "SELECT `id`,`password`,`ban` FROM `users` WHERE `email` == '" + Server::escape(username) +
                "';");
        if (q == NULL)
            return bad();

        if (q->getRowsCount() < 1) {
            delete q;
            resp << ResponseIncorrectCredentials;
            _sock->send(resp);
            return;
        }
        delete q;
        MYSQL_ROW row = q->getRow();
        std::string sid = row[0];
        std::string pass = row[1];
        std::string sban = row[2];
        if (MD5(std::string(password.rbegin(), password.rend())).hexdigest() != pass) {
            resp << ResponseIncorrectCredentials;
            _sock->send(resp);
            return;
        }
        if (std::stoi(sban) > 0) {
            resp << ResponseBanned;
            resp << (sf::Uint8) std::stoi(sban);
            _sock->send(resp);
            return;
        }
        sf::Uint64 session[] = {
                getRandom(),
                getRandom()
        };
        q = Server::instance()->query(
                "INSERT INTO `session` VALUES (NULL, NOW(), '" + sid + "', " + std::to_string(session[0]) +
                ", " + std::to_string(session[1]) + ");");
        if (q == NULL)
            return;
        delete q;
        sf::Uint8 rev = static_cast<sf::Uint8>(rand() % 2);
        resp << ResponseResultInfo;
        resp << rev;
        if (rev == 0)
            resp << (sf::Uint64) session[0] << (sf::Uint64) std::stoll(sid) << (sf::Uint64) session[1];
        else
            resp << (sf::Uint64) session[1] << (sf::Uint64) std::stoll(sid) << (sf::Uint64) session[0];
        _sock->send(resp);
        _state |= StateAuthorized;
    }

}