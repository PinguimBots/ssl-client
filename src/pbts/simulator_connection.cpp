#include "pbts/simulator_connection.hpp"

#include <QObject> // QObject::connect.

pbts::simulator_connection::simulator_connection
(
    connection_params simulator_in_params,
    connection_params simulator_out_params,
    std::function<void(fira_message::sim_to_ref::Environment)> on_simulator_receive,
    connection_params referee_in_params,
    connection_params replacer_out_params,
    std::function<void(VSSRef::ref_to_team::VSSRef_Command)> on_referee_receive
)
    : simulator_out_socket{}
    , simulator_out_addr{simulator_out_params.ip.data()}
    , simulator_out_port{simulator_out_params.port}
    , simulator_in_socket{}
    , replacer_out_socket{}
    , replacer_out_addr{replacer_out_params.ip.data()}
    , replacer_out_port{replacer_out_params.port}
    , referee_in_socket{}
{
    simulator_in_socket.bind(QHostAddress::AnyIPv4, simulator_in_params.port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    simulator_in_socket.joinMulticastGroup(QHostAddress{simulator_in_params.ip.data()});

    // Connection is automatically severed when simulator_in_socket is destroyed.
    QObject::connect(&simulator_in_socket, &QUdpSocket::readyRead,
        [
            this,
            packet = fira_message::sim_to_ref::Environment{},
            on_receive = std::move(on_simulator_receive)
        ]() mutable {
            // If we want to be pedantic we should do while(simulator_in_socket.hasPendingDatagrams()).
            // But we wont.
            const auto dg = this->simulator_in_socket.receiveDatagram();
            // NOTE: dg.data() returns a copy.
            const auto data = dg.data();

            packet.ParseFromArray(data.data(), data.size());
            on_receive(packet);
        }
    );

    referee_in_socket.bind(QHostAddress::AnyIPv4, referee_in_params.port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    referee_in_socket.joinMulticastGroup(QHostAddress{referee_in_params.ip.data()});

    QObject::connect(&referee_in_socket, &QUdpSocket::readyRead,
        [
            this,
            command = VSSRef::ref_to_team::VSSRef_Command{},
            on_receive = std::move(on_referee_receive)
        ]() mutable {
            const auto dg = this->referee_in_socket.receiveDatagram();
            const auto data = dg.data();

            command.ParseFromArray(data.data(), data.size());
            on_receive(command);
        }
    );
}

auto pbts::simulator_connection::simulator_send(const fira_message::sim_to_ref::Packet& packet) -> bool
{
    QByteArray dg;
    dg.resize(packet.ByteSize());
    bool success = packet.SerializeToArray(dg.data(), dg.size());
    if(!success) {return false;}

    auto bytes_sent = simulator_out_socket.writeDatagram(dg, simulator_out_addr, simulator_out_port);
    return bytes_sent == dg.size();
}

auto pbts::simulator_connection::replacer_send(const VSSRef::team_to_ref::VSSRef_Placement& placement) -> bool
{
    QByteArray dg;
    dg.resize(placement.ByteSize());
    bool success = placement.SerializeToArray(dg.data(), dg.size());
    if(!success) {return false;}

    auto bytes_sent = replacer_out_socket.writeDatagram(dg, replacer_out_addr, replacer_out_port);
    return bytes_sent == dg.size();
}
