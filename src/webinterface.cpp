#include "webinterface.h"
#include "light.h"
#include "changer.h"
#include "state.h"
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

namespace squidlights {

Wt::WToolBar *changersToolBar(unsigned light_idx,
                              Wt::WStackedWidget *chgrs_widg) {
  Wt::WToolBar *toolBar = new Wt::WToolBar();

  state_mutex.lock();

  unsigned chg_i = 0;
  for (const changer_t& chg : changers()) {
    Wt::WPushButton* btn = new Wt::WPushButton("Off");
    btn->setCheckable(true);
    btn->setChecked(light_active_changer(light_idx) == chg_i);

    btn->clicked().connect(bind([=]() {
      state_mutex.lock();

      for (unsigned btn_i = 0; btn_i < toolBar->count(); ++btn_i) {
        Wt::WPushButton *btn_i_widg =
            dynamic_cast<Wt::WPushButton *>(toolBar->widget(btn_i));
        assert(btn_i_widg);
        btn_i_widg->setChecked(false);
      }

      Wt::WPushButton *chg_widg =
          dynamic_cast<Wt::WPushButton *>(toolBar->widget(chg_i));
      assert(chg_widg);
      chg_widg->setChecked(true);

      chgrs_widg->setCurrentIndex(chg_i);

      state_mutex.unlock();
    }));

    toolBar->addButton(btn);

    ++chg_i;
  }

  state_mutex.unlock();

  return toolBar;
}

Wt::WWidget *changerWidget(const changer_t &chg) {
  for (const changer_arg_t& a : chg.args) {
    switch (a.ty) {
    case CHANGER_ARG_COLOR: {
      Wt::WImage *img = new Wt::WImage(Wt::WLink("resources/color_picker.png"));
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
    } break;

    case CHANGER_ARG_BOUNDED_INT:
      return new Wt::WText("TODO");
      break;
    }
  }
}

Wt::WStackedWidget* changersWidget() {
  Wt::WStackedWidget *chgrs_widg = new Wt::WStackedWidget();

  for (const changer_t& chg : changers())
    chgrs_widg->addWidget(changerWidget(chg));

  chgrs_widg->addWidget(new Wt::WText(""));
  chgrs_widg->setCurrentIndex(changers().size());

  return chgrs_widg;
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

  unsigned l_idx = 0;
  for (const light_t& l : lights()) {
    Wt::WGroupBox *group = new Wt::WGroupBox(l.desc);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(group);
    Wt::WVBoxLayout *vLayout = new Wt::WVBoxLayout(container);
    vLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WStackedWidget *chgrs_widg = changersWidget();
    vLayout->addWidget(changersToolBar(l_idx, chgrs_widg));
    vLayout->addWidget(chgrs_widg);

    menu->addItem(l.nm, group);

    ++l_idx;
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

}
