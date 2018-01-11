////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "src/ports.hpp"
#include "window.hpp"

#include <QCloseEvent>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
window::window(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);

    ui_.top->layout()->addWidget(pbus_ = new gui::pbus);
    pbus_->set(src::avail_ports());
    connect(pbus_, &gui::pbus::open, this, &window::open_device);
    connect(pbus_, &gui::pbus::close, [&](){ device_.reset(); });

    ui_.top->layout()->addWidget(casparcg_ = new gui::casparcg);
    connect(casparcg_, &gui::casparcg::open, this, &window::open_server);
    connect(casparcg_, &gui::casparcg::close, [&](){ server_.reset(); });

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
void window::open_device(const QString& name, int device)
{
    device_.reset(new src::pbus(name, device));

    ////////////////////
    connect(&*device_, &src::pbus::opened, pbus_, &gui::pbus::opened);
    connect(&*device_, &src::pbus::closed, pbus_, &gui::pbus::closed);

    connect(&*device_, &src::pbus::exec, control_, &gui::control::exec);

    ////////////////////
    connect(&*device_, &src::pbus::info, console_, &gui::console::info);
    connect(&*device_, &src::pbus::warn, console_, &gui::console::warn);
    connect(&*device_, &src::pbus::fail, console_, &gui::console::fail);
}

////////////////////////////////////////////////////////////////////////////////
void window::open_server(const QString& name, int port, int chan)
{
    server_.reset(new src::casparcg(name, port, chan));

    ////////////////////
    connect(&*server_, &src::casparcg::opened, casparcg_, &gui::casparcg::opened);
    connect(&*server_, &src::casparcg::closed, casparcg_, &gui::casparcg::closed);

    connect(casparcg_, &gui::casparcg::scan, &*server_, &src::casparcg::scan);
    connect(&*server_, &src::casparcg::scanned, casparcg_, &gui::casparcg::scanned);

    ////////////////////
    connect(&*server_, &src::casparcg::opened, control_, &gui::control::opened);
    connect(&*server_, &src::casparcg::closed, control_, &gui::control::closed);

    connect(&*server_, &src::casparcg::scanned, control_, &gui::control::scanned);

    connect(control_, &control::play  , &*server_, &src::casparcg::play  );
    connect(control_, &control::pause , &*server_, &src::casparcg::pause );
    connect(control_, &control::resume, &*server_, &src::casparcg::resume);
    connect(control_, &control::stop  , &*server_, &src::casparcg::stop  );

    ////////////////////
    connect(&*server_, &src::casparcg::info, console_, &gui::console::info);
    connect(&*server_, &src::casparcg::warn, console_, &gui::console::warn);
    connect(&*server_, &src::casparcg::fail, console_, &gui::console::fail);
}

////////////////////////////////////////////////////////////////////////////////
}
