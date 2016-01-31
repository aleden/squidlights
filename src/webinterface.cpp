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
#include <Wt/WPaintedWidget>
#include <Wt/WPainter>

using namespace std;

namespace squidlights {

class ColorPickerWidget : public Wt::WPaintedWidget {
public:
  struct _color {
    uint8_t rgb[3];
  };
private:
  int picked_x, picked_y;
  Wt::Signal<_color> colorChanged_;
  _color clr;

  constexpr static unsigned img_size = 200;
  constexpr static unsigned circle_size = 8;
  constexpr static unsigned circle_width = 2;
  constexpr static unsigned shadow_width = 4;

public:
  ColorPickerWidget(Wt::WContainerWidget *parent = 0)
      : Wt::WPaintedWidget(parent), picked_x(-1), picked_y(-1),
        colorChanged_(this) {
    // provide a default size
    resize(img_size, img_size);

    mouseWentDown().connect(this, &ColorPickerWidget::mouseDown);
  }

  const _color &color() { return clr; }

  Wt::Signal<_color>& colorChanged() {
    return colorChanged_;
  }

protected:
  void paintEvent(Wt::WPaintDevice *paintDevice) {
    Wt::WPainter painter(paintDevice);
    painter.setRenderHint(Wt::WPainter::Antialiasing);

    Wt::WPainter::Image image("resources/color_picker.png", img_size, img_size);

    painter.drawImage(0.0, 0.0, image);
    if (picked_x >= 0 && picked_y >= 0) {
      Wt::WPainterPath path;
      path.addEllipse(picked_x - circle_size / 2, picked_y - circle_size / 2,
                      circle_size, circle_size);

      painter.setShadow(Wt::WShadow(0, 0, Wt::WColor(255, 255, 255, 255), shadow_width));
      Wt::WPen pen = Wt::WPen(Wt::black);
      pen.setWidth(circle_width);
      painter.strokePath(path, pen);
    }
  }

  void mouseDown(const Wt::WMouseEvent &e) {
    cout << "mouse down" << endl;
    Wt::Coordinates c = e.widget();
    picked_x = c.x;
    picked_y = c.y;
    updateColor();
    colorChanged().emit(clr);

    update(Wt::PaintUpdate);
  }

  void hsvToRgb(double h, double s, double v, uint8_t rgb[]) {
    double r, g, b;

    int i = int(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch (i % 6) {
    case 0:
      r = v, g = t, b = p;
      break;
    case 1:
      r = q, g = v, b = p;
      break;
    case 2:
      r = p, g = v, b = t;
      break;
    case 3:
      r = p, g = q, b = v;
      break;
    case 4:
      r = t, g = p, b = v;
      break;
    case 5:
      r = v, g = p, b = q;
      break;
    }

    rgb[0] = r * 255;
    rgb[1] = g * 255;
    rgb[2] = b * 255;
  }

  void updateColor() {
    double h = static_cast<double>(picked_x) / static_cast<double>(img_size);
    double s = static_cast<double>(picked_y) / static_cast<double>(img_size);
    const double v = 1.0;

    hsvToRgb(h, s, v, clr.rgb);
  }
};

Wt::WToolBar *changersToolBar(unsigned light_idx,
                              Wt::WStackedWidget *chgrs_widg) {
  Wt::WToolBar *toolBar = new Wt::WToolBar();

  state_mutex.lock();

  unsigned chg_i = 0;
  for (const changer_t &chg : changers()) {
    Wt::WPushButton *btn = new Wt::WPushButton(chg.nm);
    btn->setToolTip(chg.desc);
    btn->setCheckable(true);
#if 0
    btn->setChecked(light_changer(light_idx) == chg_i);
#endif

    btn->clicked().connect([=](const Wt::WMouseEvent& e) {
      state_mutex.lock();

      set_changer_for_light(chg_i, light_idx);

      for (unsigned btn_i = 0; btn_i < toolBar->count(); ++btn_i) {
        if (btn_i == chg_i)
          continue;

        Wt::WPushButton *btn_i_widg =
            dynamic_cast<Wt::WPushButton *>(toolBar->widget(btn_i));
        assert(btn_i_widg);
        btn_i_widg->setChecked(false);
      }
      chgrs_widg->setCurrentIndex(chg_i);

      state_mutex.unlock();
    });

    toolBar->addButton(btn);

    ++chg_i;
  }

  state_mutex.unlock();

  return toolBar;
}

Wt::WWidget *changerWidget(changer_t &chg, unsigned l_idx) {
  Wt::WContainerWidget *container = new Wt::WContainerWidget();
  Wt::WVBoxLayout *vLayout = new Wt::WVBoxLayout();

  for (changer_arg_t &a : chg.args) {
    Wt::WGroupBox *group = new Wt::WGroupBox(a.desc);

    switch (a.ty) {
    case CHANGER_ARG_COLOR: {
      changer_arg_t* ap = &a;
#if 0
      Wt::WImage *img =
          new Wt::WImage(Wt::WLink("resources/color_picker.png"), group);
#if 0
      img->clicked().connect([=](const Wt::WMouseEvent &e) {
        cout << "clicked at [" << e.widget().x << "," << e.widget().y << ")"
             << endl;

        double h = static_cast<double>(e.widget().x) / static_cast<double>(200);
        double s = static_cast<double>(e.widget().y) / static_cast<double>(200);
        const double v = 1.0;

        uint8_t rgb[3];
        hsvToRgb(h, s, v, rgb);
        ap->color.r = rgb[0];
        ap->color.g = rgb[1];
        ap->color.b = rgb[2];

        restart_light_changer(l_idx);
      });

      img->mouseDragged().connect([=](const Wt::WMouseEvent &e) {
        cout << "dragged at [" << e.widget().x << "," << e.widget().y << ")"
             << endl;

        double h = static_cast<double>(e.widget().x) / static_cast<double>(200);
        double s = static_cast<double>(e.widget().y) / static_cast<double>(200);
        const double v = 1.0;

        uint8_t rgb[3];
        hsvToRgb(h, s, v, rgb);
        ap->color.r = rgb[0];
        ap->color.g = rgb[1];
        ap->color.b = rgb[2];

        restart_light_changer(l_idx);
      });
#endif
      img->setMaximumSize(200, 200);
#else
      ColorPickerWidget* clr_pkr = new ColorPickerWidget(group);
      clr_pkr->colorChanged().connect([=](...) {
        state_mutex.lock();
        ap->color.r = clr_pkr->color().rgb[0];
        ap->color.g = clr_pkr->color().rgb[1];
        ap->color.b = clr_pkr->color().rgb[2];

        restart_light_changer(l_idx);
        state_mutex.unlock();
      });
#endif
      break;
    }

    default:
      new Wt::WText("TODO", group);
      break;
    }

    vLayout->addWidget(group);
  }

  container->setLayout(vLayout);

  return container;
}

Wt::WStackedWidget *changersWidget(unsigned l_idx) {
  Wt::WStackedWidget *chgrs_widg = new Wt::WStackedWidget();

  for (changer_t &chg : changers())
    chgrs_widg->addWidget(changerWidget(chg, l_idx));

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
  for (const light_t &l : lights()) {
    Wt::WGroupBox *group = new Wt::WGroupBox(l.desc);

    Wt::WContainerWidget *container = new Wt::WContainerWidget(group);
    Wt::WVBoxLayout *vLayout = new Wt::WVBoxLayout(container);
    vLayout->setContentsMargins(0, 0, 0, 0);

    Wt::WStackedWidget *chgrs_widg = changersWidget(l_idx);
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
