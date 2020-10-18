#include "pbts/simulator_connection.hpp"
#include "simulator_connection.hpp"

#include <QObject> // QObject::connect.

pbts::simulator_connection::simulator_connection(
    connection_params in_params,
    connection_params out_params,
    std::function<void(fira_message::sim_to_ref::Environment)> on_receive
) : listen_socket{}, send_socket{}, out_addr{out_params.ip.data()}, out_port{out_params.port}
{
    listen_socket.bind(QHostAddress::AnyIPv4, in_params.port);
    listen_socket.joinMulticastGroup(QHostAddress{in_params.ip.data()});

    // Connection is automatically severed when listen_socket is destroyed.
    QObject::connect(&listen_socket, &QUdpSocket::readyRead,
        [
            this,
            packet = fira_message::sim_to_ref::Environment{},
            on_receive = std::move(on_receive)
        ]() mutable {
            // If we want to be pedantic we should do while(listen_socket.hasPendingDatagrams()).
            // But we wont.
            const auto dg = this->listen_socket.receiveDatagram();
            // NOTE: dg.data() returns a copy.
            const auto data = dg.data();

            packet.ParseFromArray(data.data(), data.size());
            on_receive(packet);
        }
    );
}

auto pbts::simulator_connection::send(const fira_message::sim_to_ref::Packet& packet) -> bool
{
    QByteArray dg;
    dg.resize(packet.ByteSize());
    bool success = packet.SerializeToArray(dg.data(), dg.size());
    if(!success) {return false;}

    auto bytes_sent = send_socket.writeDatagram(dg, out_addr, out_port);
    return bytes_sent != dg.size();
}
