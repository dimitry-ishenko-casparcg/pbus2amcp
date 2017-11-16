////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "asio_or_boost.hpp"
#include "common.hpp"
#include "gui/window.hpp"

#include <QApplication>

#include <chrono>
#include <csignal>
#include <exception>
#include <functional>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
try
{
    auto proc_signal = [](int n)
    {
        std::cerr << "Received signal " << n << std::endl;
        src::quit = true;
    };
    std::signal(SIGINT , proc_signal);
    std::signal(SIGTERM, proc_signal);

    ////////////////////
    asio::io_service io;
    asio::system_timer timer_gui(io);

    QApplication app(argc, argv);

    gui::window win;
    QObject::connect(&win, &gui::window::closing, [&](){ src::quit = true; });
    win.show();

    using proc = std::function<void(const asio::error_code&)>;
    proc proc_gui;

    using namespace std::chrono_literals;
    timer_gui.async_wait(proc_gui = [&](const asio::error_code&)
    {
        app.processEvents();
        if(src::quit) return;

        timer_gui.expires_at(timer_gui.expires_at() + 10ms);
        timer_gui.async_wait(proc_gui);
    });
    timer_gui.expires_from_now(0ms);

    ////////////////////
    io.run();
    return 0;
}
catch(std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
