////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "src/ports.hpp"
#include "window.hpp"

#include <QCloseEvent>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamWriter>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
window::window(QWidget* parent) : QMainWindow(parent)
{
    ui_.setupUi(this);
    dialog_.setNameFilter("XML files (*.xml);; All files (*)");

    connect(ui_.reset,   &QAction::triggered, this, &window::reset);
    connect(ui_.open,    &QAction::triggered, this, &window::open);
    connect(ui_.save,    &QAction::triggered, this, &window::save);
    connect(ui_.save_as, &QAction::triggered, this, &window::save_as);
    connect(ui_.exit,    &QAction::triggered, this, &window::close);

    ui_.top->layout()->addWidget(pbus_ = new gui::pbus(src::avail_ports()));
    connect(pbus_, &gui::pbus::changed, this, &window::changed);
    connect(pbus_, &gui::pbus::open, this, &window::open_device);
    connect(pbus_, &gui::pbus::close, [&](){ device_.reset(); });

    ui_.top->layout()->addWidget(casparcg_ = new gui::casparcg);
    connect(casparcg_, &gui::casparcg::changed, this, &window::changed);
    connect(casparcg_, &gui::casparcg::open, this, &window::open_server);
    connect(casparcg_, &gui::casparcg::close, [&](){ server_.reset(); });

    ui_.top->layout()->addWidget(control_ = new gui::control);
    connect(control_, &gui::control::changed, this, &window::changed);

    ui_.bottom->layout()->addWidget(console_ = new gui::console);
}

////////////////////////////////////////////////////////////////////////////////
void window::closeEvent(QCloseEvent* event)
{
    if(changed_ && QMessageBox::Yes == QMessageBox::question(
        this, "Save", "Do you want to save the rundown before exiting?"))
    save();

    emit closing();
    QWidget::closeEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
void window::set_title()
{
    setWindowTitle("PBus to AMCP"
        + (path_.size() ? " - " + path_ : "")
        + (changed_ ? " *" : "")
    );
}

////////////////////////////////////////////////////////////////////////////////
void window::changed()
{
    if(!ignore_changed_ && !changed_)
    {
        changed_ = true;
        set_title();
    }
}

////////////////////////////////////////////////////////////////////////////////
void window::reset()
{
    ignore_changed_ = true;
    pbus_->reset();
    casparcg_->reset();
    control_->reset();
    ignore_changed_ = false;

    path_ = "";
    changed_ = false;
    set_title();

    console_->info("New rundown");
}

////////////////////////////////////////////////////////////////////////////////
void window::open()
{
    dialog_.setAcceptMode(QFileDialog::AcceptOpen);
    if(dialog_.exec() == QDialog::Accepted)
    {
        QFile file(dialog_.selectedFiles().front());
        if(file.open(QFile::ReadOnly))
        {
            QDomDocument doc;
            QString err;
            if(doc.setContent(&file, &err))
            {
                reset();

                auto nodes = doc.elementsByTagName("rundown");
                if(nodes.size())
                {
                    auto node = nodes.item(0).toElement();

                    ignore_changed_ = true;
                    pbus_->read(node);
                    casparcg_->read(node);
                    control_->read(node);
                    ignore_changed_ = false;

                    path_ = file.fileName();
                    changed_ = false;
                    set_title();

                    console_->info("Opened rundown from " + path_);
                }
                else QMessageBox::critical(this, "Error", "Invalid XML file");
            }
            else QMessageBox::critical(this, "Error", err);
        }

        if(file.error() != QFile::NoError)
            QMessageBox::critical(this, "Error", file.errorString());
    }
}

////////////////////////////////////////////////////////////////////////////////
void window::save()
{
    if(path_.size()) write(); else save_as();
}

////////////////////////////////////////////////////////////////////////////////
void window::save_as()
{
    dialog_.setAcceptMode(QFileDialog::AcceptSave);
    if(dialog_.exec() == QDialog::Accepted)
    {
        path_ = dialog_.selectedFiles().front();
        if(path_.indexOf(".") < 0)
        {
            path_ += ".xml";
            dialog_.selectFile(path_);
        }
        write();
    }
}

////////////////////////////////////////////////////////////////////////////////
void window::write()
{
    QFile file(path_);
    if(file.open(QFile::WriteOnly | QFile::Truncate))
    {
        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);

        writer.writeStartDocument();
        writer.writeStartElement("rundown");
        writer.writeAttribute("id", "pbus2amcp");

        pbus_->write(writer);
        casparcg_->write(writer);
        control_->write(writer);

        writer.writeEndElement();
        writer.writeEndDocument();

        file.close();
    }

    if(file.error() == QFile::NoError)
    {
        changed_ = false;
        console_->info("Saved rundown to " + path_);
    }
    else QMessageBox::critical(this, "Error", file.errorString());

    set_title();
}

////////////////////////////////////////////////////////////////////////////////
void window::open_device(const QString& name, int device)
{
    device_.reset(new src::pbus(name, device));

    ////////////////////
    connect(&*device_, &src::pbus::opened, pbus_, &gui::pbus::opened);
    connect(&*device_, &src::pbus::closed, pbus_, &gui::pbus::closed);

    connect(&*device_, &src::pbus::exec, control_, &gui::control::exec);

    ////////////////////
    connect(&*device_, &src::pbus::info, console_, &gui::console::info);
    connect(&*device_, &src::pbus::warn, console_, &gui::console::warn);
    connect(&*device_, &src::pbus::fail, console_, &gui::console::fail);
}

////////////////////////////////////////////////////////////////////////////////
void window::open_server(const QString& name, int port, int chan)
{
    server_.reset(new src::casparcg(name, port, chan));

    ////////////////////
    connect(&*server_, &src::casparcg::opened, casparcg_, &gui::casparcg::opened);
    connect(&*server_, &src::casparcg::closed, casparcg_, &gui::casparcg::closed);

    connect(casparcg_, &gui::casparcg::scan, &*server_, &src::casparcg::scan);
    connect(&*server_, &src::casparcg::scanned, casparcg_, &gui::casparcg::scanned);

    ////////////////////
    connect(&*server_, &src::casparcg::opened, control_, &gui::control::opened);
    connect(&*server_, &src::casparcg::closed, control_, &gui::control::closed);

    connect(&*server_, &src::casparcg::scanned, control_, &gui::control::scanned);

    connect(control_, &control::play  , &*server_, &src::casparcg::play  );
    connect(control_, &control::pause , &*server_, &src::casparcg::pause );
    connect(control_, &control::resume, &*server_, &src::casparcg::resume);
    connect(control_, &control::stop  , &*server_, &src::casparcg::stop  );
    connect(control_, &control::clear , &*server_, &src::casparcg::clear );

    ////////////////////
    connect(&*server_, &src::casparcg::info, console_, &gui::console::info);
    connect(&*server_, &src::casparcg::warn, console_, &gui::console::warn);
    connect(&*server_, &src::casparcg::fail, console_, &gui::console::fail);
}

////////////////////////////////////////////////////////////////////////////////
}
