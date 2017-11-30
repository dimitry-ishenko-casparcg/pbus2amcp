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
#include <QTextCharFormat>

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
console::console(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);

    QFont font("");
    font.setStyleHint(QFont::Courier);

    auto format = ui_.text->currentCharFormat();
    format.setFont(font);
    ui_.text->setCurrentCharFormat(format);
}

////////////////////////////////////////////////////////////////////////////////
void console::info(const QString& text)
{
    append(text);
}

////////////////////////////////////////////////////////////////////////////////
void console::crit(const QString& text)
{
    QTextCharFormat before = ui_.text->currentCharFormat();
    QTextCharFormat format = before;
    format.setForeground(Qt::red);

    ui_.text->setCurrentCharFormat(format);
    append(text);
    ui_.text->setCurrentCharFormat(before);
}

////////////////////////////////////////////////////////////////////////////////
void console::append(const QString& text)
{
    auto bar = ui_.text->verticalScrollBar();
    bool at_max = bar->value() == bar->maximum();

    ui_.text->append(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ") + text);

    if(at_max) bar->setValue(bar->maximum());
}

////////////////////////////////////////////////////////////////////////////////
}
