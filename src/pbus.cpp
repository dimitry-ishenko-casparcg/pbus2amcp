////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "pbus.hpp"
#include <QTimer>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
pbus::pbus(const QString& name, int device, QObject* parent) :
    QObject(parent), device_(1 << device)
{
    port_.setPortName(name);
    port_.open(QSerialPort::ReadWrite);
    port_.setBaudRate(38400);
    port_.setParity(QSerialPort::OddParity);

    connect(&port_, &QSerialPort::aboutToClose, this, &pbus::closed);
    connect(&port_, &QSerialPort::aboutToClose, [&](){ emit info("PBus: Closed"); });

    connect(&port_, &QSerialPort::readyRead, this, &pbus::read);

    QTimer::singleShot(0, [&]()
    {
        if(port_.isOpen())
        {
            emit info("PBus: Opened");
            emit opened();
        }
        else
        {
            emit fail("PBus: " + port_.errorString());
            emit closed();
        }
    });
}

////////////////////////////////////////////////////////////////////////////////
void pbus::read()
{
    store_.append(port_.readAll());

    for(auto p = store_.indexOf('\r'); p >= 0; p = store_.indexOf('\r'))
    {
        auto cmd = store_.mid(0, p);
        store_.remove(0, p + 1);

        bool valid = false;
        if(cmd.size() < 7) emit warn("PBus: Invalid command " + cmd);
        else
        {
            auto devices = cmd.mid(1, 6).toInt(&valid, 16);
            if(!valid) emit warn("PBus: Invalid device " + cmd);
            else
            {
                if(!(devices & device_)) emit info("PBus: Ignoring command " + cmd);
                else
                {
                    auto value = cmd.mid(7).toInt(&valid, 16);
                    if(!valid) emit warn("PBus: Invalid value " + cmd);
                    else
                    {
                        if(cmd[0] == 'R')
                        {
                            emit info("PBus: Recall register " + QString::number(value));
                            reg_ = value;
                        }
                        else if(cmd[0] == 'T')
                        {
                            if(reg_ < 0) emit warn("PBus: Register not set");
                            else switch(static_cast<trigger>(value))
                            {
                            case trigger::play:
                                emit info("PBus: Trigger play");
                                emit exec(reg_, trigger::play);
                                break;

                            case trigger::pause:
                                emit info("PBus: Trigger pause");
                                emit exec(reg_, trigger::pause);
                                break;

                            case trigger::resume:
                                emit info("PBus: Trigger resume");
                                emit exec(reg_, trigger::resume);
                                break;

                            case trigger::stop:
                                emit info("PBus: Trigger stop");
                                emit exec(reg_, trigger::stop);
                                break;

                            default: emit warn("PBus: Invalid trigger " + cmd);
                            }
                        }
                        else emit warn("PBus: Invalid command " + cmd);
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
}
