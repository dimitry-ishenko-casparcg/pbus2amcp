////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "reg.hpp"
#include <QColor>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
reg::reg(int n, QWidget* parent) : QWidget(parent), nr_(n)
{
    ui_.setupUi(this);

    ui_.nr->setText(QString::number(nr_));
    if(nr_ >= 0 && nr_ <= 9)
    {
        ui_.nr->setBuddy(ui_.path);
        ui_.nr->setText("&" + ui_.nr->text());
    }
    reset();

    connect(ui_.play, &QToolButton::clicked, [&]()
    {
        emit play(nr_,
            ui_.path->currentText(),
            ui_.from->value(),
            ui_.to->value(),
            ui_.fade_in->isChecked(),
            ui_.fade_out->isChecked(),
            ui_.loop->isChecked()
        );
        ui_.pause->setChecked(false);
    });

    connect(ui_.pause, &QToolButton::clicked, [&]()
    {
        if(ui_.pause->isChecked()) emit pause(nr_);
        else emit resume(nr_);
    });

    connect(ui_.stop, &QToolButton::clicked, [&]()
    {
        emit stop(nr_, ui_.fade_out->isChecked());
        ui_.pause->setChecked(false);
    });
}

////////////////////////////////////////////////////////////////////////////////
void reg::reset()
{
    ui_.path->clear();
    ui_.path->clearEditText();
    ui_.from->setValue(0);
    ui_.to->setValue(0);
    ui_.fade_in->setChecked(true);
    ui_.fade_out->setChecked(true);
    ui_.loop->setChecked(false);
    ui_.pause->setChecked(false);
}

////////////////////////////////////////////////////////////////////////////////
void reg::set(const src::media& media)
{
    auto text = ui_.path->currentText();
    ui_.path->clear();

    for(auto const& read : media)
    {
        ui_.path->addItem(read.path);

        QColor color = Qt::black;
        switch(read.type)
        {
        case src::audio: color = Qt::darkMagenta; break;
        case src::still: color = Qt::blue; break;
        default: break;
        }

        ui_.path->setItemData(ui_.path->count() - 1, color, Qt::ForegroundRole);
    }

    ui_.path->setCurrentText(text);
}

////////////////////////////////////////////////////////////////////////////////
void reg::exec(src::trigger trig)
{
    using src::trigger;
    switch(trig)
    {
    case trigger::play:   ui_.play->click(); break;
    case trigger::pause:  ui_.pause->setChecked(false); ui_.pause->click(); break;
    case trigger::resume: ui_.pause->setChecked(true); ui_.pause->click(); break;
    case trigger::stop:   ui_.stop->click(); break;
    }
}

////////////////////////////////////////////////////////////////////////////////
}
