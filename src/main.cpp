////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "gui/window.hpp"
#include <QApplication>

#include <csignal>
#include <exception>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
try
{
    auto proc_signal = [](int n)
    {
        std::cerr << "Received signal " << n << std::endl;
        QApplication::quit();
    };
    std::signal(SIGINT , proc_signal);
    std::signal(SIGTERM, proc_signal);

    ////////////////////
    QApplication app(argc, argv);

    gui::window win;
    QObject::connect(&win, &gui::window::closing, &app, &QApplication::quit);
    win.show();

    ////////////////////
    return app.exec();
}
catch(std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
