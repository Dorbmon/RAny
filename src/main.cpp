#include <gtkmm.h>
#include "to_pcm.h"
#include <thread>
#include <unistd.h>
#include "gtkmm/application.h"
#include "gtkmm/object.h"
#include "import_window.h"
#include "app.h"
#include "analyzing_progress_window.h"
Glib::RefPtr<Gtk::Application> app;
void on_initial_window_close(std::optional<std::string> path) {
  if (path.has_value()) {
    // go analyze
    auto call_back = AnalyzingProgressWindow::pop_out("Analyzing...");
    // start to parse
    std::thread t ([call_back, path] () {
      auto output_filename = tmp_filename();
      convertToPCM(path.value(), output_filename, call_back);
    });
    // t.join();
    t.detach();
    
  } else {
    auto import_window = Gtk::make_managed<ImportWindow>(on_initial_window_close);
    import_window->present();
    app->add_window(*import_window);
  }
}
int main(int argc, char* argv[]) {
  app = Gtk::Application::create("org.rx.rx.rany");

  app->signal_activate().connect([] {
    auto import_window = Gtk::make_managed<ImportWindow>(on_initial_window_close);
    app->add_window(*import_window);
    import_window->present();
    // auto analyzing_window = Gtk::make_managed<AnalyzingProgressWindow>();
    // app->add_window(*analyzing_window);
    // analyzing_window->present();
  });
  app->run(argc, argv);
}