////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "pbus.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
pbus::pbus(const src::ports& ports, QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);
    for(auto const& port : ports) ui_.port->addItem(port);

    set(state::closed);
    reset();

    connect(ui_.open, &QPushButton::clicked, [&]()
    {
        set(state::opening);
        emit open(ui_.port->currentText(), ui_.device->value());
    });
    connect(ui_.close, &QPushButton::clicked, this, &pbus::close);
}

////////////////////////////////////////////////////////////////////////////////
void pbus::reset()
{
    emit close();
    ui_.port->setCurrentIndex(0);
    ui_.device->setValue(0);
}

////////////////////////////////////////////////////////////////////////////////
void pbus::set(state new_state)
{
    switch(new_state)
    {
    case state::closed:
        ui_.port->setEnabled(true);
        ui_.device->setEnabled(true);

        ui_.open->show();
        ui_.open->setEnabled(true);
        ui_.open->setText("&Open");

        ui_.close->hide();

        ui_.settings->setEnabled(true);
        break;

    case state::opening:
        ui_.port->setEnabled(false);
        ui_.device->setEnabled(false);

        ui_.open->show();
        ui_.open->setEnabled(false);
        ui_.open->setText("Opening");

        ui_.close->hide();

        ui_.settings->setEnabled(false);
        break;

    case state::opened:
        ui_.port->setEnabled(false);
        ui_.device->setEnabled(false);

        ui_.open->hide();
        ui_.close->show();

        ui_.settings->setEnabled(false);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
}
