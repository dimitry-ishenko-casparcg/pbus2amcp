////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "macro.hpp"
#include "pbus.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
pbus::pbus(const src::ports& ports, QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);
    for(auto const& port : ports) ui_.port->addItem(port);

    update();
    reset();

    connect(ui_.port, &QComboBox::currentTextChanged, this, &pbus::changed);
    connect(ui_.device, VOID(QSpinBox, valueChanged, int), this, &pbus::changed);

    ////////////////////
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
inline auto to_elem(const QDomNodeList& nodes, int index)
{ return nodes.item(index).toElement(); }

////////////////////////////////////////////////////////////////////////////////
void pbus::read(const QDomElement& parent)
{
    auto nodes = parent.elementsByTagName("pbus");
    if(nodes.size())
    {
        auto node = to_elem(nodes, 0);

        nodes = node.elementsByTagName("port");
        if(nodes.size()) ui_.port->setCurrentText(to_elem(nodes, 0).text());

        nodes = node.elementsByTagName("device");
        if(nodes.size()) ui_.device->setValue(to_elem(nodes, 0).text().toInt());
    }
}

////////////////////////////////////////////////////////////////////////////////
void pbus::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("pbus");
    writer.writeTextElement("port", ui_.port->currentText());
    writer.writeTextElement("device", QString::number(ui_.device->value()));
    writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////////////////
void pbus::set(state new_state)
{
    if(new_state != state_)
    {
        state_ = new_state;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
void pbus::update()
{
    switch(state_)
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
