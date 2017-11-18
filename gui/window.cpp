////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "ui_window.h"
#include "window.hpp"

#include <QCloseEvent>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
window::window(QWidget* parent) : QWidget(parent),
    ui(new Ui::window)
{
    ui->setupUi(this);

    ui->caspar_scan->setEnabled(false);
    ui->registers_group->setEnabled(false);

    connect(ui->serial_open, &QPushButton::clicked,
        [&](){ if(!open_) serial_open(); else serial_close(); }
    );
    connect(ui->serial_settings, &QPushButton::clicked, this, &window::serial_settings);

    connect(ui->caspar_connect, &QPushButton::clicked,
        [&](){ if(!connected_) caspar_connect(); else caspar_disconnect(); }
    );
    connect(ui->caspar_scan, &QPushButton::clicked, this, &window::caspar_scan);
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

    ui->serial_open->setText("Close");
    ui->serial_settings->setEnabled(false);
    open_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void window::serial_close()
{
    // close

    ui->serial_open->setText("Open");
    ui->serial_settings->setEnabled(true);
    open_ = false;
}

////////////////////////////////////////////////////////////////////////////////
void window::serial_settings()
{
    // settings
}

////////////////////////////////////////////////////////////////////////////////
void window::caspar_connect()
{
    // connect

    ui->caspar_connect->setText("Disconnect");
    ui->caspar_scan->setEnabled(true);
    ui->registers_group->setEnabled(true);
    connected_ = true;
}

////////////////////////////////////////////////////////////////////////////////
void window::caspar_disconnect()
{
    // disconnect

    ui->caspar_connect->setText("Connect");
    ui->caspar_scan->setEnabled(false);
    ui->registers_group->setEnabled(false);
    connected_ = false;
}

////////////////////////////////////////////////////////////////////////////////
void window::caspar_scan()
{
    // scan
}

////////////////////////////////////////////////////////////////////////////////
}
