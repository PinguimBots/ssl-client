#ifndef GRSIMCLIENT_H
#define GRSIMCLIENT_H

#include <string_view>

#include <QObject>
#include <QUdpSocket>

#include "pb/command.pb.h"
#include "pb/common.pb.h"
#include "pb/packet.pb.h"
#include "pb/replacement.pb.h"

class GrSim_Client : public QObject
{
    Q_OBJECT
public:
    explicit GrSim_Client(std::uint16_t port, std::string_view addr, QObject *parent = 0);
    bool sendCommand(double velL, double velR, int id, bool isYellow);
    QHostAddress _addr;
    quint16 _port;

signals:

public slots:
    void readyRead();

private:
    QUdpSocket *socket;

};

#endif // GRSIMCLIENT_H
