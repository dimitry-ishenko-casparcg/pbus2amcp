////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef SRC_AMCP_HPP
#define SRC_AMCP_HPP

////////////////////////////////////////////////////////////////////////////////
#include <QByteArray>
#include <QByteArrayList>
#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QtNetwork/QAbstractSocket>

////////////////////////////////////////////////////////////////////////////////
namespace src
{

////////////////////////////////////////////////////////////////////////////////
class amcp : public QObject
{
    Q_OBJECT

public:
    ////////////////////
    amcp(QAbstractSocket&, const QByteArray& cmd, QObject* parent = nullptr);

signals:
    ////////////////////
    void done(QByteArrayList&);

    void info(const QString&);
    void crit(const QString&);

private:
    ////////////////////
    QAbstractSocket& socket_;
    QList<QMetaObject::Connection> conn_;

    enum class state { none, data_one, data, error_one, done } state_ = state::none;

    QByteArray store_;
    QByteArrayList data_;

    void read();

    QString pre(QString message);

    void quit();
    void fail(const QString&);
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
