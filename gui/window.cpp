////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "gui/casparcg.hpp"
#include "gui/pbus.hpp"
#include "reg.hpp"
#include "window.hpp"

#include <QCloseEvent>

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
    ui_.layout->insertWidget(1, new gui::casparcg);

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
}
