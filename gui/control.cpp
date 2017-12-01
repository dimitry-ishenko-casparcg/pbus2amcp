////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "control.hpp"
#include "reg.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
control::control(QWidget* parent) : QWidget(parent)
{
    ui_.setupUi(this);
    closed();

    for(int n = 0; n < 100; ++n)
    {
        auto widget = new reg(n);
        ui_.reges->layout()->addWidget(widget);

        connect(widget, &reg::play  , this, &control::play  );
        connect(widget, &reg::pause , this, &control::pause );
        connect(widget, &reg::resume, this, &control::resume);
        connect(widget, &reg::stop  , this, &control::stop  );
    }
}

////////////////////////////////////////////////////////////////////////////////
void control::opened() { ui_.reges->setEnabled(true); }
void control::closed() { ui_.reges->setEnabled(false); }

////////////////////////////////////////////////////////////////////////////////
void control::scanned(const src::media& media)
{
    auto reges = ui_.reges->layout();
    for(int n = 0; n < reges->count(); ++n)
        static_cast<reg*>(reges->itemAt(n)->widget())->set(media);
}

////////////////////////////////////////////////////////////////////////////////
}
