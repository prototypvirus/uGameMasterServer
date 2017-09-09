//
// Created by symbx on 09.09.17.
//

#include <utils/Logger.h>
#include <Constants.h>
#include "core/Server.h"

namespace uGame {

    Server* Server::_instance = NULL;

    Server::Server() :
        _listener(),
        _sockets(),
        _selector() {
        L_INFO("Initialization");
        _db = new MYSQL();
        if(!mysql_init(_db)) {
            L_ERR("Error creating MySQL object!");
            _db = NULL;
            return;
        }
        if(!mysql_real_connect(_db, S_HOST, S_USER, S_PASS, S_BASE, 0, NULL, 0)) {
            L_ERR("Can't connect to "+std::string(S_HOST)+" as "+std::string(S_USER));
            delete _db;
            _db = NULL;
            return;
        }
        L_INFO("Connected to "+std::string(S_HOST)+" as "+std::string(S_USER));

        _version = "1.0.0";
        Query* q = Server::instance()->query("SELECT `value` FROM `settings` WHERE `name` == 'version`;");
        if(q == NULL)
            return;
        if(q->getRowsCount() < 1) {
            delete q;
            return;
        }
        _version = q->getRow()[0];
        delete q;
        q = Server::instance()->query("SELECT `value` FROM `settings` WHERE `name` == 'maintenance`;");
    }

    Server::~Server() {
        if(_db != NULL) {
            L_INFO("Close connection.");
            mysql_close(_db);
            delete _db;
        }
    }

    void Server::run() {
        _running = true;
        _listener.listen(PORT);
        _selector.add(_listener);
        sf::Time timeout = sf::seconds(2.0f);
        while(_running) {
            if(_selector.wait(timeout)) {
                if(_selector.isReady(_listener)) {
                    sf::TcpSocket* sock = new sf::TcpSocket();
                    if(_listener.accept(*sock) == sf::Socket::Done) {
                        _sockets.push_back(new Client(sock));
                        _selector.add(*sock);
                    }else
                        delete sock;
                } else {
                    for (auto it = _sockets.begin(); it != _sockets.end(); ++it) {
                        Client* cli = *it;
                        if(_selector.isReady(*cli->socket())) {
                            cli->receive();
                        }
                        if(cli->toKick()) {
                            _selector.remove(*cli->socket());
                            _sockets.remove(cli);
                            delete cli;
                        }
                    }
                }
            }
        }
    }

    void Server::stop() {
        _running = false;
    }

    Server *Server::instance() {
        if(Server::_instance == NULL)
            Server::_instance = new Server();
        return Server::_instance;
    }

    Query *Server::query(const std::string &q) {
        if(mysql_query(_db, q.c_str()))
            return NULL;
        return new Query(std::move(mysql_store_result(_db)));
    }

    std::string &Server::getVersion() {
        return _version;
    }

    bool Server::isMaintenance() {
        return _maintenance;
    }

    std::string Server::escape(const std::string &v) {
        char* to = new char[v.length()*2+1];
        mysql_real_escape_string(_instance->_db, to, v.c_str(), v.length());
        return std::move(std::string(to));
    }
}