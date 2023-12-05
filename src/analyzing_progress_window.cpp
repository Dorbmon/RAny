#include "analyzing_progress_window.h"
#include "glibmm/main.h"
#include <iostream>
#include <mutex>

AnalyzingProgressWindow::AnalyzingProgressWindow(): AnalyzingProgressWindow("") {}
AnalyzingProgressWindow::AnalyzingProgressWindow(std::string title) {
    set_title(title);
    set_child(progress_bar);
    progress_bar.set_hexpand(true);
    progress_bar.set_vexpand(true);
    progress_bar.set_margin(10);
}
void AnalyzingProgressWindow::set_progress(double progress) {
    progress_bar.set_fraction(progress);
    if (progress > 1) {
        std:std::call_once(close_flag, [this] () {
            Glib::signal_timeout().connect_seconds([this]() -> bool {
                close();
                return false;
            }, 2);
        });
        
    }
}

std::function<void(double)> AnalyzingProgressWindow::get_progress_setter() {
    return std::bind(&AnalyzingProgressWindow::set_progress, this, std::placeholders::_1);
}