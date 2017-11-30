////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "console.hpp"

#include <QDateTime>
#include <QFont>
#include <QScrollBar>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
console::console(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);

    QFont font("");
    font.setStyleHint(QFont::Courier);

    info_ = ui_.text->currentCharFormat();
    info_.setFont(font);

    crit_ = info_;
    crit_.setForeground(Qt::red);
}

////////////////////////////////////////////////////////////////////////////////
void console::append(const QString& text, const QTextCharFormat& format)
{
    auto bar = ui_.text->verticalScrollBar();
    bool at_max = bar->value() == bar->maximum();

    ui_.text->setCurrentCharFormat(format);
    ui_.text->append(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ") + text);

    if(at_max) bar->setValue(bar->maximum());
}

////////////////////////////////////////////////////////////////////////////////
}
