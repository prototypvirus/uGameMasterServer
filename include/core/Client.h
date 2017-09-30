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
            RequestHandShake = 1,
            RequestAuthorize/*,
            RequestChangePassword,
            RequestDeleteAccount*/ //For future...
        };
        enum PacketIDResponse {
            ResponseMaintenance = 1,
            ResponseUpdateRequire,
            ResponseContinue,
            ResponseIncorrectCredentials,
            ResponseBanned,
            ResponseResultInfo/*,
            ResponseSuccess*/ //For future...
        };
        enum State {
            StateNone = 0,
            StateHandshake = 1,
            StateAuthorized = 2,
            StateKick = 4
        };
        Client(sf::TcpSocket* sock);
        ~Client();
        sf::TcpSocket* socket();
        void receive();
        bool toKick();

    protected:
        sf::TcpSocket* _sock;
        sf::Uint16 _state;
        void bad();
        unsigned long getRandom();
        void handshake(sf::Packet in);
        void authorize(sf::Packet in);
    };
}

#endif //UGAMEMASTERSERVER_CLIENT_H
