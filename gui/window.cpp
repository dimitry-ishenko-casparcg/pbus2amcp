////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "gui/pbus.hpp"
#include "reg.hpp"
#include "window.hpp"

#include <QCloseEvent>
#include <QMessageBox>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

constexpr int nr_min = 1;
constexpr int nr_max = 56;

////////////////////////////////////////////////////////////////////////////////
window::window(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);
    ui_.layout->insertWidget(0, new gui::pbus);

    ui_.layout->insertWidget(1, ui_casparcg_ = new gui::casparcg);
    connect(ui_casparcg_, &gui::casparcg::open, this, &window::casparcg_open);
    connect(ui_casparcg_, &gui::casparcg::close, this, &window::casparcg_close);

    for(int nr = nr_min; nr <= nr_max; ++nr)
        ui_.registers->layout()->addWidget(new reg(nr));
}

////////////////////////////////////////////////////////////////////////////////
void window::closeEvent(QCloseEvent* event)
{
    emit closing();
    QWidget::closeEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void window::casparcg_open(const QString& name, quint16 port)
{
    casparcg_.reset(new src::casparcg(name, port));

    connect(&*casparcg_, &src::casparcg::opened, ui_casparcg_, &gui::casparcg::opened);
    connect(&*casparcg_, &src::casparcg::closed, ui_casparcg_, &gui::casparcg::closed);

    connect(&*casparcg_, &src::casparcg::failed,
        [&](const QString& error){ QMessageBox::critical(this, "CasparCG", error); }
    );
    connect(&*casparcg_, &src::casparcg::failed, ui_casparcg_, &gui::casparcg::closed);
}

////////////////////////////////////////////////////////////////////////////////
void window::casparcg_close() { casparcg_.reset(); }

////////////////////////////////////////////////////////////////////////////////
}
