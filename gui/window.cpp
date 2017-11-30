////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "gui/pbus.hpp"
#include "window.hpp"

#include <QCloseEvent>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
window::window(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);

    ui_.top->layout()->addWidget(new gui::pbus);

    ui_.top->layout()->addWidget(casparcg_ = new gui::casparcg);
    connect(casparcg_, &gui::casparcg::open, this, &window::casparcg_open);
    connect(casparcg_, &gui::casparcg::close, this, &window::casparcg_close);

    ui_.top->layout()->addWidget(control_ = new gui::control);

    ui_.bottom->layout()->addWidget(console_ = new gui::console);
}

////////////////////////////////////////////////////////////////////////////////
void window::closeEvent(QCloseEvent* event)
{
    emit closing();
    QWidget::closeEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void window::casparcg_open(const QString& name, quint16 port, quint16 chan)
{
    server_.reset(new src::casparcg(name, port, chan));

    ////////////////////
    connect(&*server_, &src::casparcg::opened, casparcg_, &gui::casparcg::opened);
    connect(&*server_, &src::casparcg::closed, casparcg_, &gui::casparcg::closed);
    connect(&*server_, &src::casparcg::failed, casparcg_, &gui::casparcg::closed);

    connect(&*server_, &src::casparcg::info, console_, &gui::console::info);
    connect(&*server_, &src::casparcg::failed, console_, &gui::console::crit);

    connect(casparcg_, &gui::casparcg::scan, &*server_, &src::casparcg::scan);
    connect(&*server_, &src::casparcg::scanned, casparcg_, &gui::casparcg::scanned);

    ////////////////////
    connect(&*server_, &src::casparcg::opened, control_, &gui::control::opened);
    connect(&*server_, &src::casparcg::closed, control_, &gui::control::closed);
    connect(&*server_, &src::casparcg::failed, control_, &gui::control::closed);

    connect(&*server_, &src::casparcg::scanned, control_, &gui::control::scanned);

    connect(control_, &control::play  , &*server_, &src::casparcg::play  );
    connect(control_, &control::pause , &*server_, &src::casparcg::pause );
    connect(control_, &control::resume, &*server_, &src::casparcg::resume);
    connect(control_, &control::stop  , &*server_, &src::casparcg::stop  );
}

////////////////////////////////////////////////////////////////////////////////
void window::casparcg_close() { server_.reset(); }

////////////////////////////////////////////////////////////////////////////////
}
