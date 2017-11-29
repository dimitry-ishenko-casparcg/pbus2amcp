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
reg::reg(int layer, QWidget* parent) : QWidget(parent),
    layer_(layer)
{
    ui_.setupUi(this);
    ui_.nr->setText(QString::number(layer_));

    if(layer_ >= 0 && layer_ <= 9)
    {
        ui_.nr->setBuddy(ui_.path);
        ui_.nr->setText("&" + ui_.nr->text());
    }
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
}
