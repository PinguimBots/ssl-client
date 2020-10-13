#include "net/grSim_client.h"
#include <cstdint>

#include <QString>

GrSim_Client::GrSim_Client(std::uint16_t port, std::string_view addr, QObject* parent) :
    QObject(parent)
{
    // create a QUDP socket
    socket = new QUdpSocket(this);

    this->_addr.setAddress( addr.data() );
    this->_port = quint16(port);

    socket->bind(this->_addr, this->_port);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

bool GrSim_Client::sendCommand(double velL, double velR, int id, bool isYellow){
    double zero = 0.0;
    fira_message::sim_to_ref::Packet packet;

    fira_message::sim_to_ref::Command* command = packet.mutable_cmd()->add_robot_commands();
    command->set_id(id);
    command->set_yellowteam(isYellow);
    command->set_wheel_left(velL);
    command->set_wheel_right(velR);

    QByteArray dgram;
    dgram.resize(packet.ByteSize());
    packet.SerializeToArray(dgram.data(), dgram.size());

    auto sent_successfully = socket->writeDatagram(dgram, this->_addr, this->_port) > -1;
    return sent_successfully;
}

void GrSim_Client::readyRead(){
    // when data comes in
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);

    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
}
