////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "reg.hpp"
#include "src/macro.hpp"

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

    connect(ui_.path, &QComboBox::currentTextChanged, this, &reg::changed);
    connect(ui_.from, VOID(QSpinBox, valueChanged, int), this, &reg::changed);
    connect(ui_.to, VOID(QSpinBox, valueChanged, int), this, &reg::changed);
    connect(ui_.fade_in, &QToolButton::toggled, this, &reg::changed);
    connect(ui_.fade_out, &QToolButton::toggled, this, &reg::changed);
    connect(ui_.loop, &QToolButton::toggled, this, &reg::changed);

    ////////////////////
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
inline auto to_elem(const QDomNodeList& nodes, int index)
{ return nodes.item(index).toElement(); }

////////////////////////////////////////////////////////////////////////////////
void reg::read(const QDomElement& parent)
{
    auto nodes = parent.elementsByTagName("register");
    for(int n = 0; n < nodes.size(); ++n)
    {
        auto node = nodes.item(n).toElement();
        if(nr_ == node.attribute("nr").toInt())
        {
            auto nodes = node.elementsByTagName("path");
            if(nodes.size()) ui_.path->setCurrentText(to_elem(nodes, 0).text());

            nodes = node.elementsByTagName("segment");
            if(nodes.size())
            {
                ui_.from->setValue(to_elem(nodes, 0).attribute("from").toInt());
                ui_.to->setValue(to_elem(nodes, 0).attribute("to").toInt());
            }

            ui_.fade_in->setChecked(node.elementsByTagName("fade-in").size());
            ui_.fade_out->setChecked(node.elementsByTagName("fade-out").size());
            ui_.loop->setChecked(node.elementsByTagName("loop").size());

            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void reg::write(QXmlStreamWriter& writer)
{
    if(ui_.path->currentText().size())
    {
        writer.writeStartElement("register");
        auto nr = ui_.nr->text();
        if(nr.startsWith("&")) nr.remove(0, 1);
        writer.writeAttribute("nr", nr);
        writer.writeTextElement("path", ui_.path->currentText());
        auto from = ui_.from->value(), to = ui_.to->value();
        if(from || to)
        {
            writer.writeStartElement("segment");
            if(from) writer.writeAttribute("from", QString::number(from));
            if(to) writer.writeAttribute("to", QString::number(to));
            writer.writeEndElement();
        }
        if(ui_.fade_in->isChecked()) writer.writeEmptyElement("fade-in");
        if(ui_.fade_out->isChecked()) writer.writeEmptyElement("fade-out");
        if(ui_.loop->isChecked()) writer.writeEmptyElement("loop");
        writer.writeEndElement();
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
