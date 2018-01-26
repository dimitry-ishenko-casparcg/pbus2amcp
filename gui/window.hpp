////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#ifndef GUI_WINDOW_HPP
#define GUI_WINDOW_HPP

////////////////////////////////////////////////////////////////////////////////
#include "gui/casparcg.hpp"
#include "gui/console.hpp"
#include "gui/control.hpp"
#include "gui/pbus.hpp"
#include "src/casparcg.hpp"
#include "src/pbus.hpp"
#include "ui_window.h"

#include <QFileDialog>
#include <QMainWindow>
#include <QScopedPointer>
#include <QString>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
class window : public QMainWindow
{
    Q_OBJECT

public:
    ////////////////////
    explicit window(QWidget* parent = nullptr);

signals:
    ////////////////////
    void closing();

protected:
    ////////////////////
    virtual void closeEvent(QCloseEvent*) override;

private:
    ////////////////////
    Ui::window ui_;

    QFileDialog dialog_;

    QString path_;
    bool changed_ = false;
    bool ignore_changed_ = false;

    void set_title();

    void changed();
    void reset();
    void open();
    void save();
    void save_as();
    void write();

    gui::pbus* pbus_;
    QScopedPointer<src::pbus> device_;

    gui::casparcg* casparcg_;
    QScopedPointer<src::casparcg> server_;

    gui::control* control_;
    gui::console* console_;

    void open_device(const QString& name, int device);
    void open_server(const QString& name, int port, int chan);
};


////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
