////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "casparcg.hpp"
#include "macro.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
casparcg::casparcg(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);

    update();
    reset();

    connect(ui_.name, &QLineEdit::textChanged, this, &casparcg::changed);
    connect(ui_.port, VOID(QSpinBox, valueChanged, int), this, &casparcg::changed);
    connect(ui_.chan, VOID(QSpinBox, valueChanged, int), this, &casparcg::changed);

    ////////////////////
    connect(ui_.open, &QPushButton::clicked, [&]()
    {
        set(state::opening);
        emit open(ui_.name->text(), ui_.port->value(), ui_.chan->value());
    });
    connect(ui_.close, &QPushButton::clicked, this, &casparcg::close);

    connect(ui_.scan, &QPushButton::clicked, [&]()
    {
        scanning(true);
        emit scan();
    });
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::reset()
{
    emit close();
    ui_.name->setText("localhost");
    ui_.port->setValue(5250);
    ui_.chan->setValue(1);
}

////////////////////////////////////////////////////////////////////////////////
inline auto to_elem(const QDomNodeList& nodes, int index)
{ return nodes.item(index).toElement(); }

////////////////////////////////////////////////////////////////////////////////
void casparcg::read(const QDomElement& parent)
{
    auto nodes = parent.elementsByTagName("casparcg");
    if(nodes.size())
    {
        auto node = to_elem(nodes, 0);

        nodes = node.elementsByTagName("hostname");
        if(nodes.size()) ui_.name->setText(to_elem(nodes, 0).text());

        nodes = node.elementsByTagName("port");
        if(nodes.size()) ui_.port->setValue(to_elem(nodes, 0).text().toInt());

        nodes = node.elementsByTagName("channel");
        if(nodes.size()) ui_.chan->setValue(to_elem(nodes, 0).text().toInt());
    }
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("casparcg");
    writer.writeTextElement("hostname", ui_.name->text());
    writer.writeTextElement("port", QString::number(ui_.port->value()));
    writer.writeTextElement("channel", QString::number(ui_.chan->value()));
    writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::set(state new_state)
{
    if(new_state != state_)
    {
        state_ = new_state;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::scanning(bool new_value)
{
    if(new_value != scanning_)
    {
        scanning_ = new_value;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
void casparcg::update()
{
    switch(state_)
    {
    case state::closed:
        ui_.name->setEnabled(true);
        ui_.port->setEnabled(true);
        ui_.chan->setEnabled(true);

        ui_.open->show();
        ui_.open->setEnabled(true);
        ui_.open->setText("&Connect");

        ui_.close->hide();

        ui_.scan->setEnabled(false);
        ui_.scan->setText("Sca&n");
        break;

    case state::opening:
        ui_.name->setEnabled(false);
        ui_.port->setEnabled(false);
        ui_.chan->setEnabled(false);

        ui_.open->show();
        ui_.open->setEnabled(false);
        ui_.open->setText("Connecting");

        ui_.close->hide();

        ui_.scan->setEnabled(false);
        ui_.scan->setText("Sca&n");
        break;

    case state::opened:
        ui_.name->setEnabled(false);
        ui_.port->setEnabled(false);
        ui_.chan->setEnabled(false);

        ui_.open->hide();
        ui_.close->show();

        if(scanning_)
        {
            ui_.scan->setEnabled(false);
            ui_.scan->setText("Scanning");
        }
        else
        {
            ui_.scan->setEnabled(true);
            ui_.scan->setText("Sca&n");
        }
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
}
