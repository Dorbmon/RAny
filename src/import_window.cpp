#include "import_window.h"
#include "gtkmm/enums.h"
#include "gtkmm/filedialog.h"
#include <optional>

ImportWindow::ImportWindow(): ImportWindow(nullptr) {}

ImportWindow::ImportWindow(std::function<void(std::optional<std::string>)> on_finish): on_finish(on_finish) {
    set_title("Import Your Audio File");
    set_default_size(250, 250);
    this->vbox.set_orientation(Gtk::Orientation::VERTICAL);
    this->choose_file_button.set_label("Browse File");
    
    this->choose_file_button.signal_clicked().connect(sigc::mem_fun(*this, &ImportWindow::on_choose_file_button_clicked));
    this->selected_file.set_hexpand(true);

    this->choose_hbox.set_orientation(Gtk::Orientation::HORIZONTAL);
    this->choose_hbox.append(this->selected_file);
    this->choose_hbox.append(this->choose_file_button);

    this->submit_button.set_label("Submit");
    this->submit_button.signal_clicked().connect([this] {
        close();
        if(this->on_finish != nullptr) {
            auto file_uri = this->selected_file.get_text();
            this->on_finish(file_uri.empty() ? std::nullopt : std::make_optional(file_uri));
        }
    });
    this->vbox.append(this->choose_hbox);
    this->vbox.append(this->submit_button);
    set_child(this->vbox);
}
void ImportWindow::on_choose_file_button_clicked() {
    auto dialog = Gtk::FileDialog::create();
    dialog->set_title("Please choose your audio file");
    dialog->set_accept_label("Select");
    dialog->open([dialog, this] (const Glib::RefPtr<Gio::AsyncResult> &result) {
        auto file = dialog->open_finish(result);
        if(file != nullptr) {
            std::string file_path = file->get_path();
            this->selected_file.set_text(file_path);
        }
    });
}