////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef SRC_CASPARCG_HPP
#define SRC_CASPARCG_HPP

////////////////////////////////////////////////////////////////////////////////
#include "media.hpp"

#include <QByteArrayList>
#include <QObject>
#include <QString>
#include <QtNetwork/QTcpSocket>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
class casparcg : public QObject
{
    Q_OBJECT

public:
    ////////////////////
    casparcg(const QString& name, int port, int chan, QObject* parent = nullptr);

    ////////////////////
    void scan();

    void   play(int layer, const QString& path, int from, int to, bool fade_in, bool fade_out, bool loop);
    void  pause(int layer);
    void resume(int layer);
    void   stop(int layer, bool fade_out);

signals:
    ////////////////////
    void opened();
    void closed();

    void info(const QString&);
    void warn(const QString&);
    void fail(const QString&);

    void scanned(const media&);

private:
    ////////////////////
    QTcpSocket socket_;
    int chan_;

    void exec(const QByteArray&);
    void amcp_done(const QByteArrayList&);
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
