#include "gtkmm/filechooser.h"
#include <gtkmm.h>

class ImportWindow : public Gtk::Window
{
public:
  ImportWindow();
private:
  Gtk::Box vbox;

  Gtk::Button choose_file_button;
  Gtk::Label button_label;
  Gtk::Entry selected_file;
  Gtk::Box choose_hbox;

  void on_choose_file_button_clicked();
};


