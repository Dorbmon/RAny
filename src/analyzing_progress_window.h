#include "gtkmm/object.h"
#include "gtkmm/progressbar.h"
#include "app.h"
#include <functional>
#include <gtkmm.h>
#include <mutex>

class AnalyzingProgressWindow : public Gtk::Window
{
public:
  AnalyzingProgressWindow();
  AnalyzingProgressWindow(std::string title);
  std::function<void(double)> get_progress_setter();
  static std::function<void(double)> pop_out(std::string title = "") {
    auto window = Gtk::make_managed<AnalyzingProgressWindow>(title);
    app->add_window(*window);
    window->present();
    return window->get_progress_setter();
  }
private:
  std::once_flag close_flag;
  Gtk::ProgressBar progress_bar;
  void set_progress(double progress);
};


