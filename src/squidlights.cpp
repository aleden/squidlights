#include "squidlightswidget.h"
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WHBoxLayout>
#include <Wt/WBootstrapTheme>
#include <Wt/WCssTheme>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

using namespace Wt;

Wt::WApplication *createApplication(const Wt::WEnvironment &env) {
  WApplication* app = new Wt::WApplication(env);

  // Choice of theme: defaults to bootstrap3 but can be overridden using
  // a theme parameter (for testing)
  const std::string *themePtr = env.getParameter("theme");
  std::string theme;
  if (!themePtr)
    theme = "bootstrap3";
  else
    theme = *themePtr;

  if (theme == "bootstrap3") {
    Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(app);
    bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
    bootstrapTheme->setResponsive(true);
    app->setTheme(bootstrapTheme);

    // load the default bootstrap3 (sub-)theme
    app->useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");
  } else if (theme == "bootstrap2") {
    Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(app);
    bootstrapTheme->setResponsive(true);
    app->setTheme(bootstrapTheme);
  } else
    app->setTheme(new Wt::WCssTheme(theme));

  Wt::WHBoxLayout *layout = new Wt::WHBoxLayout(app->root());
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(new SquidLightsWidget());

  app->setTitle("Squid Lights");

  app->useStyleSheet("style/everywidget.css");
  app->useStyleSheet("style/dragdrop.css");
  app->useStyleSheet("style/combostyle.css");
  app->useStyleSheet("style/pygments.css");
  app->useStyleSheet("style/layout.css");

  return app;
}

int main(int argc, char **argv) {
  /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */
  return WRun(argc, argv, &createApplication);
}

