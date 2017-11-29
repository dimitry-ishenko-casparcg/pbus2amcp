////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "casparcg.hpp"

////////////////////////////////////////////////////////////////////////////////
#define VOID(cl, fn, ...) static_cast<void(cl::*)(__VA_ARGS__)>(&cl::fn)

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
casparcg::casparcg(const QString& name, quint16 port, QObject* parent) :
    QObject(parent)
{
    connect(&socket_, &QTcpSocket::connected, this, &casparcg::opened);
    connect(&socket_, &QTcpSocket::disconnected, this, &casparcg::closed);

    connect(&socket_, VOID(QTcpSocket, error, QTcpSocket::SocketError),
        [&](){ emit failed(socket_.errorString()); }
    );

    socket_.connectToHost(name, port);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::scan()
{
    amcp_.reset(new amcp(socket_, "CLS"));

    connect(&*amcp_, &amcp::success, this, &casparcg::proc_scan);
    connect(&*amcp_, &amcp::failure, this, &casparcg::failed);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::proc_scan(const QByteArrayList& data)
{
    src::media media;

    for(auto read : data)
    {
        if(!read.size() || read[0] != '"') continue;

        auto pos = read.indexOf('"', 1);
        if(pos < 0) continue;

        auto path = read.mid(1, pos - 1);

        read.remove(0, pos + 1);
        read = read.trimmed();

        pos = read.indexOf(' ');
        if(pos < 0) continue;

        auto type = read.mid(0, pos).toLower();
             if(type == "audio") media << medium { audio, path };
        else if(type == "movie") media << medium { movie, path };
        else if(type == "still") media << medium { still, path };
    }

    emit scanned(media);
}

////////////////////////////////////////////////////////////////////////////////
}
