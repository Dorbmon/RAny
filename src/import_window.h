#include <functional>
#include <gtkmm.h>
#include <optional>

class ImportWindow : public Gtk::Window
{
public:
  ImportWindow();
  ImportWindow(std::function<void(std::optional<std::string>)> on_finish);
private:
  Gtk::Box vbox;

  Gtk::Button choose_file_button;
  Gtk::Entry selected_file;
  Gtk::Box choose_hbox;

  Gtk::Button submit_button;
  void on_choose_file_button_clicked();

  std::function<void(std::optional<std::string>)> on_finish;


};


