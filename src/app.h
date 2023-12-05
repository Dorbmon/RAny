#pragma once

#include "gtkmm/application.h"
extern Glib::RefPtr<Gtk::Application> app;
#include <filesystem>
#include "tool.h"

static std::string tmp_filename() {
    auto random_name = generateRandomString(15);
    auto tmp_dir = std::filesystem::temp_directory_path();
    tmp_dir.append("random_name");
    return tmp_dir.string();
}