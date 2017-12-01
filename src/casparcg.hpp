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

    void   play(int n, const QString& path, int from, int to, bool fade_in, bool fade_out, bool loop);
    void  pause(int n);
    void resume(int n);
    void   stop(int n, bool fade_out);

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

    inline auto nr(int n) { return QByteArray::number(n); }
    inline auto chan(int n) { return nr(chan_) + '-' + nr(n); }

    void exec(const QByteArray&);
    void scan_done(const QByteArrayList&);
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
