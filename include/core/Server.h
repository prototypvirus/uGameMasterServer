//
// Created by symbx on 09.09.17.
//

#ifndef UGAMEMASTERSERVER_SERVER_H
#define UGAMEMASTERSERVER_SERVER_H

#include <list>
#include <mysql/mysql.h>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Packet.hpp>
#include "Client.h"
#include "Query.h"

namespace uGame {
    class Server {
    public:
        Server();
        ~Server();
        void run();
        void stop();
        static Server* instance();
        Query* query(const std::string& q);
        std::string& getVersion();
        bool isMaintenance();
        static std::string escape(const std::string& v);

    protected:
        MYSQL* _db;
        sf::TcpListener _listener;
        std::list<Client*> _sockets;
        sf::SocketSelector _selector;
        bool _running;
        static Server* _instance;
        std::string _version;
        bool _maintenance;

        friend Query;
    };
}

#endif //UGAMEMASTERSERVER_SERVER_H
