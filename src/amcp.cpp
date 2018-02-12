////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "amcp.hpp"
#include "macro.hpp"

#include <QTimer>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
amcp::amcp(QAbstractSocket& socket, const QByteArray& cmd, QObject* parent) :
    QObject(parent), socket_(socket)
{
    connect(&socket_, VOID(QAbstractSocket, error, QAbstractSocket::SocketError),
        [&](){ emit fail(socket_.errorString()); }
    );
    connect(&socket_, &QAbstractSocket::readyRead, this, &amcp::read);

    // delay write to make sure
    // all connections have been established
    QTimer::singleShot(0, [=](){ socket_.write(cmd + "\r\n"); });
}

////////////////////////////////////////////////////////////////////////////////
void amcp::read()
{
    store_.append(socket_.readAll());

    for(auto p = store_.indexOf("\r\n"); p >= 0; p = store_.indexOf("\r\n"))
    {
        auto cmd = store_.mid(0, p);
        store_.remove(0, p + 2);

        switch(state_)
        {
        case state::init:
            {
                auto code = cmd.mid(0, cmd.indexOf(' '));

                     if(code == "200") state_ = state::read_data;
                else if(code == "201") state_ = state::read_datum;
                else if(code == "202") set_done();
                else if(code == "400") state_ = state::fail;
                else if(code == "401") set_fail("Illegal video channel");
                else if(code == "402") set_fail("Parameter missing");
                else if(code == "403") set_fail("Illegal parameter");
                else if(code == "404") set_fail("Media file not found");
                else if(code == "500") set_fail("Internal server error");
                else if(code == "501") set_fail("Internal server error");
                else if(code == "502") set_fail("Media file unreadable");
                else if(code == "503") set_fail("Access error");
                else set_fail("Invalid response: " + cmd);
            }
            break;

        case state::read_datum:
            data_ << cmd;
            set_done();
            break;

        case state::read_data:
            if(cmd.size()) data_ << cmd; else set_done();
            break;

        case state::fail:
            set_fail("Invalid command: " + cmd);
            break;

        default: break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void amcp::set_done()
{
    emit done(data_);
    state_ = state::done;
}

////////////////////////////////////////////////////////////////////////////////
void amcp::set_fail(const QString& message)
{
    emit fail(message);

    emit done(QByteArrayList());
    state_ = state::done;
}

////////////////////////////////////////////////////////////////////////////////
}
