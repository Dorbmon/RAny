#include <gtkmm.h>
#include "import_window.h"
int main(int argc, char* argv[]) {
  auto app = Gtk::Application::create("org.rx.rx.rany");

  return app->make_window_and_run<ImportWindow>(argc, argv);
}