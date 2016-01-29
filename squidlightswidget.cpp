#include "squidlightswidget.h"
#include "lights.h"

#include <Wt/WHBoxLayout>
#include <Wt/WMenu>
#include <Wt/WNavigationBar>
#include <Wt/WStackedWidget>
#include <Wt/WText>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WGroupBox>
#include <Wt/WToolBar>
#include <Wt/WImage>

using namespace std;

static Wt::WPushButton *createColorButton(const char *className,
                                          const Wt::WString &text) {
  Wt::WPushButton *button = new Wt::WPushButton();
  button->setTextFormat(Wt::XHTMLText);
  button->setText(text);
  button->addStyleClass(className);
  button->setCheckable(true);
  button->setChecked(false);
  return button;
}

Wt::WToolBar* makeLightControlToolBar(Wt::WStackedWidget* sttngs) {
  Wt::WToolBar *toolBar = new Wt::WToolBar();

  Wt::WPushButton *fixedBtn = createColorButton("btn-inverse", "Fixed");
  Wt::WPushButton *clrWashBtn = createColorButton("", "Color Wash");
  Wt::WPushButton *redBtn = createColorButton("btn-danger", "Red");
  Wt::WPushButton *greenBtn = createColorButton("btn-success", "Green");
  Wt::WPushButton *blueBtn = createColorButton("btn-primary", "Blue");
  Wt::WPushButton *orangeBtn = createColorButton("btn-warning", "Orange");
  Wt::WPushButton *offBtn = new Wt::WPushButton("Off");

  fixedBtn->clicked().connect(std::bind([=]() {
    sttngs->setCurrentIndex(0);

    clrWashBtn->setChecked(false);
    redBtn->setChecked(false);
    greenBtn->setChecked(false);
    blueBtn->setChecked(false);
    orangeBtn->setChecked(false);
    offBtn->setChecked(false);
  }));
  clrWashBtn->clicked().connect(std::bind([=]() {
    sttngs->setCurrentIndex(1);

    fixedBtn->setChecked(false);
    redBtn->setChecked(false);
    greenBtn->setChecked(false);
    blueBtn->setChecked(false);
    orangeBtn->setChecked(false);
    offBtn->setChecked(false);
  }));
  redBtn->clicked().connect(std::bind([=]() {
    sttngs->setCurrentIndex(2);

    fixedBtn->setChecked(false);
    clrWashBtn->setChecked(false);
    greenBtn->setChecked(false);
    blueBtn->setChecked(false);
    orangeBtn->setChecked(false);
    offBtn->setChecked(false);
  }));
  greenBtn->clicked().connect(std::bind([=]() {
    sttngs->setCurrentIndex(3);

    fixedBtn->setChecked(false);
    clrWashBtn->setChecked(false);
    redBtn->setChecked(false);
    blueBtn->setChecked(false);
    orangeBtn->setChecked(false);
    offBtn->setChecked(false);
  }));
  blueBtn->clicked().connect(std::bind([=]() {
    sttngs->setCurrentIndex(4);

    fixedBtn->setChecked(false);
    clrWashBtn->setChecked(false);
    redBtn->setChecked(false);
    greenBtn->setChecked(false);
    orangeBtn->setChecked(false);
    offBtn->setChecked(false);
  }));
  orangeBtn->clicked().connect(std::bind([=]() {
    sttngs->setCurrentIndex(5);

    fixedBtn->setChecked(false);
    clrWashBtn->setChecked(false);
    redBtn->setChecked(false);
    greenBtn->setChecked(false);
    blueBtn->setChecked(false);
    offBtn->setChecked(false);
  }));
  offBtn->clicked().connect(std::bind([=]() {
    sttngs->setCurrentIndex(6);

    fixedBtn->setChecked(false);
    clrWashBtn->setChecked(false);
    redBtn->setChecked(false);
    greenBtn->setChecked(false);
    blueBtn->setChecked(false);
    orangeBtn->setChecked(false);
  }));

  offBtn->setCheckable(true);
  offBtn->setChecked(true);

  toolBar->addButton(fixedBtn);
  toolBar->addButton(clrWashBtn);
  toolBar->addSeparator();
  toolBar->addButton(redBtn);
  toolBar->addButton(greenBtn);
  toolBar->addButton(blueBtn);
  toolBar->addButton(orangeBtn);
  toolBar->addSeparator();
  toolBar->addButton(offBtn);

  return toolBar;
}

Wt::WStackedWidget* settingsWidget() {
  Wt::WStackedWidget *sttngs = new Wt::WStackedWidget();

  Wt::WImage *img =
      new Wt::WImage(Wt::WLink("resources/color_picker.png"));
  img->clicked().connect(std::bind([=](const Wt::WMouseEvent &e) {
    cout << "clicked at " << boost::lexical_cast<std::string>(e.widget().x)
         << "," << boost::lexical_cast<std::string>(e.widget().y) << ")"
         << endl;
  }, std::placeholders::_1));
  img->mouseDragged().connect(std::bind([=](const Wt::WMouseEvent &e) {
    cout << "dragged at " << boost::lexical_cast<std::string>(e.widget().x)
         << "," << boost::lexical_cast<std::string>(e.widget().y) << ")"
         << endl;
  }, std::placeholders::_1));
  sttngs->addWidget(img);
  sttngs->addWidget(new Wt::WText("color wash"));
  sttngs->addWidget(new Wt::WText("red"));
  sttngs->addWidget(new Wt::WText("green"));
  sttngs->addWidget(new Wt::WText("blue"));
  sttngs->addWidget(new Wt::WText("orange"));
  sttngs->addWidget(new Wt::WText("off"));

  sttngs->setCurrentIndex(6);

  return sttngs;
}

SquidLightsWidget::SquidLightsWidget() : WContainerWidget() {
  setOverflow(OverflowHidden);

  Wt::WHBoxLayout *hLayout = new Wt::WHBoxLayout(this);

  Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();
  contentsStack->addStyleClass("contents");
  contentsStack->setOverflow(WContainerWidget::OverflowAuto);

  Wt::WMenu *menu = new Wt::WMenu(contentsStack);
  menu->addStyleClass("nav-pills nav-stacked submenu");
  menu->setWidth(200);

  hLayout->addWidget(menu);
  hLayout->addWidget(contentsStack, 1);

  for (const char** p = lights; *p; ++p) {
    Wt::WGroupBox *group = new Wt::WGroupBox("Light Settings");

    Wt::WContainerWidget *container = new Wt::WContainerWidget(group);
    Wt::WVBoxLayout *vLayout = new Wt::WVBoxLayout(container);
    vLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WStackedWidget* sttngs = settingsWidget();
    vLayout->addWidget(makeLightControlToolBar(sttngs));
    vLayout->addWidget(sttngs);

    menu->addItem(*p, group);
  }

#if 0
  navigation_ = new Wt::WNavigationBar();
  navigation_->addStyleClass("main-nav");
  navigation_->setTitle("Squid Lights");
  navigation_->setResponsive(true);

  contentsStack_ = new Wt::WStackedWidget();

  Wt::WAnimation animation(Wt::WAnimation::Fade,
			   Wt::WAnimation::Linear,
			   200);
  contentsStack_->setTransitionAnimation(animation, true);

  /*
   * Setup the top-level menu
   */
  Wt::WMenu *menu = new Wt::WMenu(contentsStack_, 0);
  menu->setInternalPathEnabled();
  menu->setInternalBasePath("/");

  navigation_->addMenu(menu);

  /*
   * Add it all inside a layout
   */
  Wt::WVBoxLayout *layout = new Wt::WVBoxLayout(this);
  layout->addWidget(navigation_);
  layout->addWidget(contentsStack_, 1);
  layout->setContentsMargins(0, 0, 0, 0);
#endif
}
