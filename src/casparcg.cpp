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
casparcg::casparcg(const QString& name, quint16 port, quint16 chan, QObject* parent) :
    QObject(parent), chan_(chan)
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
    auto scan = new amcp(socket_, "CLS", this);

    connect(scan, &amcp::success, this, &casparcg::proc);
    connect(scan, &amcp::failure, this, &casparcg::failed);

    connect(scan, &amcp::success, scan, &amcp::deleteLater);
    connect(scan, &amcp::failure, scan, &amcp::deleteLater);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::proc(const QByteArrayList& data)
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

    emit scanned(media);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::play(int layer, const QString& path, int from, int to, bool fade_in, bool fade_out, bool loop)
{
    QByteArray cmd;
    QTextStream stream(&cmd);

    stream << "PLAY " << chan_ << "-" << layer << " \"" << path << "\"";
    if(loop) stream << " LOOP";
    if(fade_in) stream << " MIX 15";
    if(from > 0) stream << " SEEK " << from;
    if(to > from) stream << " LENGTH " << (to - from);
    stream.flush();

    auto play = new amcp(socket_, cmd, this);
    connect(play, &amcp::success, play, &amcp::deleteLater);
    connect(play, &amcp::failure, this, &casparcg::failed);
    connect(play, &amcp::success, play, &amcp::deleteLater);

    if(fade_out)
    {
        cmd.clear();
        QTextStream stream(&cmd);

        stream << "LOADBG " << chan_ << "-" << layer << " EMPTY MIX 15 AUTO";
        stream.flush();

        play = new amcp(socket_, cmd, this);
        connect(play, &amcp::success, play, &amcp::deleteLater);
        connect(play, &amcp::failure, this, &casparcg::failed);
        connect(play, &amcp::success, play, &amcp::deleteLater);
    }
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::pause(int layer)
{
    QByteArray cmd;
    QTextStream stream(&cmd);

    stream << "PAUSE " << chan_ << "-" << layer;
    stream.flush();

    auto pause = new amcp(socket_, cmd, this);
    connect(pause, &amcp::success, pause, &amcp::deleteLater);
    connect(pause, &amcp::failure, this, &casparcg::failed);
    connect(pause, &amcp::success, pause, &amcp::deleteLater);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::resume(int layer)
{
    QByteArray cmd;
    QTextStream stream(&cmd);

    stream << "RESUME " << chan_ << "-" << layer;
    stream.flush();

    auto resume = new amcp(socket_, cmd, this);
    connect(resume, &amcp::success, resume, &amcp::deleteLater);
    connect(resume, &amcp::failure, this, &casparcg::failed);
    connect(resume, &amcp::success, resume, &amcp::deleteLater);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::stop(int layer, bool fade_out)
{
    QByteArray cmd;
    QTextStream stream(&cmd);

    if(fade_out) stream << "PLAY " << chan_ << "-" << layer << " EMPTY MIX 15";
    else stream << "STOP " << chan_ << "-" << layer;
    stream.flush();

    auto stop = new amcp(socket_, cmd, this);
    connect(stop, &amcp::success, stop, &amcp::deleteLater);
    connect(stop, &amcp::failure, this, &casparcg::failed);
    connect(stop, &amcp::success, stop, &amcp::deleteLater);
}

////////////////////////////////////////////////////////////////////////////////
}
