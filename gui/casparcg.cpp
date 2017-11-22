////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "casparcg.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
casparcg::casparcg(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);
    set(state::closed);

    connect(ui_.open, &QPushButton::clicked, [&]()
    {
        set(state::opening);
        emit open(ui_.name->text(), ui_.port->value());
    });
    connect(ui_.close, &QPushButton::clicked, this, &casparcg::close);
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::set(state new_state)
{
    switch(new_state)
    {
    case state::closed:
        ui_.name->setEnabled(true);
        ui_.port->setEnabled(true);

        ui_.open->show();
        ui_.open->setEnabled(true);
        ui_.open->setText("&Connect");

        ui_.close->hide();

        ui_.scan->setEnabled(false);
        ui_.scan->setText("Sc&an");
        break;

    case state::opening:
        ui_.name->setEnabled(false);
        ui_.port->setEnabled(false);

        ui_.open->show();
        ui_.open->setEnabled(false);
        ui_.open->setText("Connecting");

        ui_.close->hide();

        ui_.scan->setEnabled(false);
        ui_.scan->setText("Sc&an");
        break;

    case state::opened:
        ui_.name->setEnabled(false);
        ui_.port->setEnabled(false);

        ui_.open->hide();
        ui_.close->show();

        ui_.scan->setEnabled(true);
        ui_.scan->setText("Sc&an");
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
}
