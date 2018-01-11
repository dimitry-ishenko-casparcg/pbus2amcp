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
    for(;;)
    {
        auto read = port_.readLine();
        if(!read.size()) break;

        store_ += std::move(read);
        if(!store_.endsWith("\r")) continue;

        store_.chop(1);
        bool valid = false;
        if(store_.size() < 7) emit warn("PBus: Invalid command " + store_);
        else
        {
            auto devices = store_.mid(1, 6).toInt(&valid, 16);
            if(!valid) emit warn("PBus: Invalid device " + store_);
            else
            {
                if(!(devices & device_)) emit info("PBus: Ignoring command " + store_);
                else
                {
                    auto hex = store_.mid(7);
                    auto arg = hex.toInt(&valid, 16);
                    if(!valid) emit warn("PBus: Invalid arg " + store_);
                    else
                    {
                        if(store_[0] == 'R')
                        {
                            emit info("PBus: Recall register " + hex.toLower());
                            reg_ = arg;
                        }
                        else if(store_[0] == 'T')
                        {
                            if(reg_ < 0) emit warn("PBus: Invalid register");
                            else switch(static_cast<trigger>(arg))
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

                            default: emit warn("PBus: Invalid trigger " + store_);
                            }
                        }
                        else emit warn("PBus: Unsupported command " + store_);
                    }
                }
            }
        }

        store_.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////
}
