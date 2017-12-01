////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "amcp.hpp"
#include "casparcg.hpp"

#include <QTextStream>

////////////////////////////////////////////////////////////////////////////////
#define VOID(cl, fn, ...) static_cast<void(cl::*)(__VA_ARGS__)>(&cl::fn)

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
casparcg::casparcg(const QString& name, int port, int chan, QObject* parent) :
    QObject(parent), chan_(chan)
{
    connect(&socket_, &QTcpSocket::connected, this, &casparcg::opened);
    connect(&socket_, &QTcpSocket::connected, [&](){ emit info("CasparCG: Connected"); });

    connect(&socket_, &QTcpSocket::disconnected, this, &casparcg::closed);
    connect(&socket_, &QTcpSocket::disconnected, [&](){ emit info("CasparCG: Disconnected"); });

    connect(&socket_, VOID(QTcpSocket, error, QTcpSocket::SocketError), [&]()
    {
        emit fail("CasparCG: " + socket_.errorString());
        emit closed();
    });

    socket_.connectToHost(name, port);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::scan()
{
    emit info("CasparCG: Scanning");

    auto scan = new amcp(socket_, "CLS", this);
    connect(scan, &amcp::done, this, &casparcg::scan_done);

    connect(scan, &amcp::done, scan, &amcp::deleteLater);
    connect(scan, &amcp::fail, this, &casparcg::warn);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::play(int n, const QString& path, int from, int to, bool fade_in, bool fade_out, bool loop)
{
    QByteArray play = "PLAY " + chan(n) + " \"" + path.toLatin1() + "\"";
    if(loop)      play += " LOOP";
    if(fade_in)   play += " MIX 15";
    if(from > 0)  play += " SEEK " + nr(from);
    if(to > from) play += " LENGTH " + nr(to - from);
    exec(play);

    if(fade_out) exec("LOADBG " + chan(n) + " EMPTY MIX 15 AUTO");
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::pause(int n) { exec("PAUSE " + chan(n)); }
void casparcg::resume(int n) { exec("RESUME " + chan(n)); }

////////////////////////////////////////////////////////////////////////////////
void casparcg::stop(int n, bool fade_out)
{
    if(fade_out) exec("PLAY " + chan(n) + " EMPTY MIX 15");
    else exec("STOP " + chan(n));
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::exec(const QByteArray& cmd)
{
    emit info("CasparCG: Executing " + cmd);

    auto amcp = new src::amcp(socket_, cmd, this);
    connect(amcp, &amcp::done, amcp, &amcp::deleteLater);
    connect(amcp, &amcp::fail, this, &casparcg::warn);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::scan_done(const QByteArrayList& data)
{
    src::media media;

    for(auto read : data)
    {
        if(!read.size() || read[0] != '"') continue;

        auto pos = read.indexOf('"', 1);
        if(pos < 0) continue;

        auto path = read.mid(1, pos - 1).replace('\\', '/');

        read.remove(0, pos + 1);
        read = read.trimmed();

        pos = read.indexOf(' ');
        if(pos < 0) continue;

        auto type = read.mid(0, pos).toLower();
             if(type == "audio") media << medium { audio, path };
        else if(type == "movie") media << medium { movie, path };
        else if(type == "still") media << medium { still, path };
    }

    ////////////////////
    emit info(QString("CasparCG: Discovered %1 items").arg(media.size()));
    emit scanned(media);
}

////////////////////////////////////////////////////////////////////////////////
}
