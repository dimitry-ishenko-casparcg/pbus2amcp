////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef GUI_CONTROL_HPP
#define GUI_CONTROL_HPP

////////////////////////////////////////////////////////////////////////////////
#include "src/media.hpp"
#include "src/trigger.hpp"
#include "ui_control.h"

#include <QtXml/QDomElement>
#include <QXmlStreamWriter>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
class control : public QWidget
{
    Q_OBJECT

public:
    ////////////////////
    explicit control(QWidget* parent = nullptr);

    ////////////////////
    void reset();
    void read(const QDomElement&);
    void write(QXmlStreamWriter&);

    void opened() { set(state::opened); }
    void closed() { set(state::closed); }

    void scanned(const src::media&);
    void exec(int n, src::trigger);

signals:
    ////////////////////
    void changed();

    void   play(int n, const QString& path, int from, int to, bool fade_in, bool fade_out, bool loop);
    void  pause(int n);
    void resume(int n);
    void   stop(int n, bool fade_out);

    void clear();

private:
    ////////////////////
    Ui::control ui_;

    enum class state { closed, opened };
    state state_ = state::closed;
    void set(state);

    void update();
};

////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
