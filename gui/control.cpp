////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "clear.hpp"
#include "control.hpp"
#include "reg.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
control::control(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);

    update();
    reset();

    auto clear = new gui::clear();
    ui_.reges->layout()->addWidget(clear);
    connect(clear, &gui::clear::play, this, &control::clear);

    for(int n = 1; n <= 100; ++n)
    {
        auto reg = new gui::reg(n);
        ui_.reges->layout()->addWidget(reg);

        connect(reg, &gui::reg::play  , this, &control::play  );
        connect(reg, &gui::reg::pause , this, &control::pause );
        connect(reg, &gui::reg::resume, this, &control::resume);
        connect(reg, &gui::reg::stop  , this, &control::stop  );
    }
}

////////////////////////////////////////////////////////////////////////////////
void control::reset()
{
    auto reges = ui_.reges->layout();
    for(int n = 1; n < reges->count(); ++n)
    {
        auto reg = qobject_cast<gui::reg*>(reges->itemAt(n)->widget());
        if(reg) reg->reset();
    }
}

////////////////////////////////////////////////////////////////////////////////
void control::set(state new_state)
{
    if(new_state != state_)
    {
        state_ = new_state;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
void control::update()
{
    switch(state_)
    {
    case state::closed:
        ui_.reges->setEnabled(false);
        break;

    case state::opened:
        ui_.reges->setEnabled(true);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
void control::scanned(const src::media& media)
{
    auto reges = ui_.reges->layout();
    for(int n = 1; n < reges->count(); ++n)
    {
        auto reg = qobject_cast<gui::reg*>(reges->itemAt(n)->widget());
        if(reg) reg->set(media);
    }
}

////////////////////////////////////////////////////////////////////////////////
void control::exec(int n, src::trigger trig)
{
    auto reges = ui_.reges->layout();
    if(n == 0)
    {
        auto clear = qobject_cast<gui::clear*>(reges->itemAt(n)->widget());
        if(clear) clear->exec(trig);
    }
    else if(n > 0 && n < reges->count())
    {
        auto reg = qobject_cast<gui::reg*>(reges->itemAt(n)->widget());
        if(reg) reg->exec(trig);
    }
}

////////////////////////////////////////////////////////////////////////////////
}
