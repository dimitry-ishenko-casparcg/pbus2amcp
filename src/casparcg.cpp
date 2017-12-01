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
    connect(scan, &amcp::done, this, &casparcg::amcp_done);

    connect(scan, &amcp::done, scan, &amcp::deleteLater);
    connect(scan, &amcp::fail, this, &casparcg::warn);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::play(int layer, const QString& path, int from, int to, bool fade_in, bool fade_out, bool loop)
{
    QByteArray play;
    QTextStream stream(&play);

    stream << "PLAY " << chan_ << "-" << layer << " \"" << path << "\"";
    if(loop) stream << " LOOP";
    if(fade_in) stream << " MIX 15";
    if(from > 0) stream << " SEEK " << from;
    if(to > from) stream << " LENGTH " << (to - from);

    stream.flush();
    exec(play);

    if(fade_out)
    {
        play.clear();
        QTextStream stream(&play);
        stream << "LOADBG " << chan_ << "-" << layer << " EMPTY MIX 15 AUTO";

        stream.flush();
        exec(play);
    }
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::pause(int layer)
{
    QByteArray pause;
    QTextStream stream(&pause);
    stream << "PAUSE " << chan_ << "-" << layer;

    stream.flush();
    exec(pause);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::resume(int layer)
{
    QByteArray resume;
    QTextStream stream(&resume);
    stream << "RESUME " << chan_ << "-" << layer;

    stream.flush();
    exec(resume);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::stop(int layer, bool fade_out)
{
    QByteArray stop;
    QTextStream stream(&stop);

    if(fade_out) stream << "PLAY " << chan_ << "-" << layer << " EMPTY MIX 15";
    else stream << "STOP " << chan_ << "-" << layer;

    stream.flush();
    exec(stop);
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
void casparcg::amcp_done(const QByteArrayList& data)
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
