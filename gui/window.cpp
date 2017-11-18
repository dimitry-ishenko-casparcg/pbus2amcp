////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "reg.hpp"
#include "ui_window.h"
#include "window.hpp"

#include <QCloseEvent>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

constexpr int nr_min = 1;
constexpr int nr_max = 56;

////////////////////////////////////////////////////////////////////////////////
window::window(QWidget* parent) : QWidget(parent),
    ui(new Ui::window)
{
    ui->setupUi(this);

    ui->casparcg_scan->setEnabled(false);
    ui->commands->setEnabled(false);
    ui->registers->setEnabled(false);

    for(int nr = nr_min; nr <= nr_max; ++nr)
        ui->registers->layout()->addWidget(new reg(nr));

    connect(ui->serial_open, &QPushButton::clicked,
        [&](){ if(!open_) serial_open(); else serial_close(); }
    );
    connect(ui->serial_settings, &QPushButton::clicked, this, &window::serial_settings);

    connect(ui->casparcg_connect, &QPushButton::clicked,
        [&](){ if(!connected_) casparcg_connect(); else casparcg_disconnect(); }
    );
    connect(ui->casparcg_scan, &QPushButton::clicked, this, &window::casparcg_scan);
}

////////////////////////////////////////////////////////////////////////////////
window::~window()
{
    delete ui;
}

////////////////////////////////////////////////////////////////////////////////
void window::closeEvent(QCloseEvent* event)
{
    emit closing();
    QWidget::closeEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void window::serial_open()
{
    // open

    ui->serial_open->setText("C&lose");
    ui->serial_settings->setEnabled(false);
    open_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void window::serial_close()
{
    // close

    ui->serial_open->setText("&Open");
    ui->serial_settings->setEnabled(true);
    open_ = false;
}

////////////////////////////////////////////////////////////////////////////////
void window::serial_settings()
{
    // settings
}

////////////////////////////////////////////////////////////////////////////////
void window::casparcg_connect()
{
    // connect

    ui->casparcg_connect->setText("D&isconnect");
    ui->casparcg_scan->setEnabled(true);
    ui->registers->setEnabled(true);
    connected_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void window::casparcg_disconnect()
{
    // disconnect

    ui->casparcg_connect->setText("&Connect");
    ui->casparcg_scan->setEnabled(false);
    ui->registers->setEnabled(false);
    connected_ = false;
}

////////////////////////////////////////////////////////////////////////////////
void window::casparcg_scan()
{
    // scan
}

////////////////////////////////////////////////////////////////////////////////
}
