//
// Created by symbx on 09.09.17.
//

#ifndef UGAMEMASTERSERVER_CLIENT_H
#define UGAMEMASTERSERVER_CLIENT_H

#include <SFML/Network/TcpSocket.hpp>

namespace uGame {
    class Client {
    public:
        enum PacketIDRequest {
            HandShake = 1,
            Authorize/*,
            ChangePassword,
            DeleteAccount*/ //For future...
        };
        enum PacketIDResponce {
            Maintenance = 1,
            UpdateRequire,
            Continue,
            IncorrectCredentials,
            Banned,
            ResultInfo
        };
        Client(sf::TcpSocket* sock);
        ~Client();
        sf::TcpSocket* socket();
        void receive();
        bool toKick();

    protected:
        sf::TcpSocket* _sock;
        bool _kick;
        void bad();
        unsigned long getRandom();
    };
}

#endif //UGAMEMASTERSERVER_CLIENT_H
