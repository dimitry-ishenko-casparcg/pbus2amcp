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
#include "src/casparcg.hpp"
#include "gui/control.hpp"
#include "ui_window.h"

#include <QScopedPointer>
#include <QWidget>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
class window : public QWidget
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

    gui::casparcg* ui_casparcg_;
    QScopedPointer<src::casparcg> casparcg_;

    gui::control* ui_control_;

    void casparcg_open(const QString& name, quint16 port, quint16 chan);
    void casparcg_close();
};


////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
#endif
