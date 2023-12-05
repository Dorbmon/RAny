#include "import_window.h"
#include "gtkmm/dialog.h"
#include "gtkmm/enums.h"
#include "gtkmm/filechoosernative.h"
#include <iostream>
ImportWindow::ImportWindow() {
    set_title("Import Your Audio File");
    set_default_size(250, 250);
    this->vbox.set_orientation(Gtk::Orientation::VERTICAL);
    this->choose_file_button.set_child(this->button_label);
    this->button_label.set_label("Browse File");
    
    this->choose_file_button.signal_clicked().connect(sigc::mem_fun(*this, &ImportWindow::on_choose_file_button_clicked));
    this->selected_file.set_hexpand(true);

    this->choose_hbox.set_orientation(Gtk::Orientation::HORIZONTAL);
    this->choose_hbox.append(this->selected_file);
    this->choose_hbox.append(this->choose_file_button);

    this->vbox.append(this->choose_hbox);
    set_child(this->vbox);
}
void ImportWindow::on_choose_file_button_clicked() {
    auto dialog = Gtk::FileChooserNative::create("Please choose the audio file", 
                                   Gtk::FileChooser::Action::OPEN,
                                   "Choose",
                                   "Cancel");
    dialog->signal_response().connect([=, this](int v) {
        if (v == Gtk::ResponseType::ACCEPT) {
            std::cout << dialog->get_file()->get_parse_name() << std::endl;
            this->selected_file.set_text(dialog->get_file()->get_parse_name());
        }
        
    });
    dialog->show();
}