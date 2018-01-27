////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef GUI_CASPARCG_HPP
#define GUI_CASPARCG_HPP

////////////////////////////////////////////////////////////////////////////////
#include "ui_casparcg.h"

#include <QtXml/QDomElement>
#include <QWidget>
#include <QXmlStreamWriter>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
class casparcg : public QWidget
{
    Q_OBJECT

public:
    ////////////////////
    explicit casparcg(QWidget* parent = nullptr);

    ////////////////////
    void reset();
    void read(const QDomElement&);
    void write(QXmlStreamWriter&);

    void opened() { set(state::opened); }
    void closed() { set(state::closed); }

    void scanned() { scanning(false); }

signals:
    ////////////////////
    void changed();

    void open(const QString& name, quint16 port, quint16 chan);
    void close();

    void scan();

private:
    ////////////////////
    Ui::casparcg ui_;

    enum class state { closed, opening, opened };
    state state_ = state::closed;
    void set(state);

    bool scanning_ = false;
    void scanning(bool);

    void update();
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
