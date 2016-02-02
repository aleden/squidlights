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
#include <Wt/WSlider>
#include <tuple>
#include <boost/format.hpp>
#include <algorithm>

using namespace std;

namespace squidlights {

class ColorViewWidget : public Wt::WPaintedWidget {
public:
  struct _color {
    uint8_t rgb[3];
  };

  constexpr static int steps = 32;
  constexpr static int img_w = 512;
  constexpr static int img_h = 128;
  constexpr static int line_width = 1;

  // saturation "step"
  int ss;

  // value "step"
  int vs;

  int picked_x, picked_y;
  Wt::Signal<_color> colorChanged_;
  _color clr;

public:
  ColorViewWidget(Wt::WContainerWidget *parent = 0)
      : Wt::WPaintedWidget(parent), ss(steps - 1), vs(steps - 1), picked_x(-1),
        picked_y(-1), colorChanged_(this) {
    // provide a default size
    resize(img_w, img_h);

    mouseWentUp().connect(this, &ColorViewWidget::mouseUp);
    mouseDragged().connect(this, &ColorViewWidget::mouseDrag);
  }

  Wt::Signal<_color> &colorChanged() { return colorChanged_; }

  void saturationChange(int i) {
    ss = i;
    updateColor();
  }

  void valueChange(int i) {
    vs = i;
    updateColor();
  }

protected:
  void paintEvent(Wt::WPaintDevice *paintDevice) {
    Wt::WPainter painter(paintDevice);
    painter.setRenderHint(Wt::WPainter::Antialiasing);

    Wt::WPainter::Image image(
        (boost::format("resources/color_picker/%d-%d.png") % ss % vs).str(),
        img_w, img_h);

    painter.drawImage(0.0, 0.0, image);
    if (picked_x >= 0 && picked_y >= 0) {
      painter.setShadow(Wt::WShadow(
          0, 0,
          Wt::WColor(255 - clr.rgb[0], 255 - clr.rgb[1], 255 - clr.rgb[2], 255),
          3.0));
      Wt::WPen pen = Wt::WPen(Wt::WColor(255 - clr.rgb[0], 255 - clr.rgb[1],
                                         255 - clr.rgb[2], 255));
      pen.setWidth(line_width);
      painter.setPen(pen);
      painter.drawLine(picked_x, 0, picked_x, img_h);
    }
  }

  void mouseUp(const Wt::WMouseEvent &e) {
    Wt::Coordinates c = e.widget();
    picked_x = c.x;
    picked_y = c.y;
    updateColor();
  }

  void mouseDrag(const Wt::WMouseEvent &e) {
    Wt::Coordinates c = e.widget();
    picked_x = c.x;
    picked_y = c.y;

    double h = static_cast<double>(picked_x) / static_cast<double>(img_w);
    double s = static_cast<double>(ss) / static_cast<double>(steps);
    double v = static_cast<double>(vs) / static_cast<double>(steps);

    hsvToRgb(h, s, v, clr.rgb);

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
    double h = static_cast<double>(picked_x) / static_cast<double>(img_w);
    double s = static_cast<double>(ss) / static_cast<double>(steps);
    double v = static_cast<double>(vs) / static_cast<double>(steps);

    hsvToRgb(h, s, v, clr.rgb);

    update(Wt::PaintUpdate);

    colorChanged().emit(clr);
  }
};

tuple<Wt::WContainerWidget *, ColorViewWidget *>
colorPickerWidget(Wt::WContainerWidget *parent = nullptr) {
  Wt::WContainerWidget *cntr = new Wt::WContainerWidget(parent);
  cntr->resize(ColorViewWidget::img_w, Wt::WLength::Auto);

  Wt::WVBoxLayout *layout = new Wt::WVBoxLayout();

  ColorViewWidget *clrvwidg = new ColorViewWidget();
  layout->addWidget(clrvwidg);

  Wt::WSlider *satsld = new Wt::WSlider(Wt::Horizontal);
  satsld->setToolTip("Saturation");
  satsld->resize(ColorViewWidget::img_w, 30);
  satsld->setRange(0, ColorViewWidget::steps - 1);
  satsld->setValue(clrvwidg->ss);
  satsld->sliderMoved().connect(clrvwidg, &ColorViewWidget::saturationChange);
  layout->addWidget(satsld);

  Wt::WSlider *valsld = new Wt::WSlider(Wt::Horizontal);
  valsld->setToolTip("Value");
  valsld->resize(ColorViewWidget::img_w, 30);
  valsld->setRange(0, ColorViewWidget::steps - 1);
  valsld->setValue(clrvwidg->vs);
  valsld->sliderMoved().connect(clrvwidg, &ColorViewWidget::valueChange);
  layout->addWidget(valsld);

  cntr->setLayout(layout);

  return make_tuple(cntr, clrvwidg);
}

Wt::WToolBar *changersToolBar(unsigned light_idx,
                              Wt::WStackedWidget *chgrs_widg) {
  Wt::WToolBar *toolBar = new Wt::WToolBar();

  unsigned chg_i = 0;
  for (const changer_t &chg : changers()) {
    if (chg_i != 0)
      toolBar->addSeparator();

    Wt::WPushButton *btn = new Wt::WPushButton(chg.nm);
    btn->setToolTip(chg.desc);
    btn->setCheckable(true);

    btn->clicked().connect([=](const Wt::WMouseEvent &e) {
      state_mutex.lock();

      set_changer_for_light(chg_i, light_idx);
      if (changers()[chg_i].args.empty())
        run_light_changer(light_idx);

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

  return toolBar;
}

Wt::WWidget *changerWidget(changer_t &chg, unsigned l_idx) {
  Wt::WContainerWidget *container = new Wt::WContainerWidget();
  Wt::WVBoxLayout *vLayout = new Wt::WVBoxLayout();

  for (changer_arg_t &a : chg.args) {
    Wt::WGroupBox *group = new Wt::WGroupBox(a.desc);
    changer_arg_t *ap = &a;

    switch (a.ty) {
    case CHANGER_ARG_COLOR: {
      Wt::WContainerWidget *contw;
      ColorViewWidget *clrpckv;
      tie(contw, clrpckv) = colorPickerWidget(group);
      clrpckv->setToolTip("Hue");

      clrpckv->colorChanged().connect(
          std::bind([=](const ColorViewWidget::_color &new_clr) {
                      state_mutex.lock();
                      ap->color.r = new_clr.rgb[0];
                      ap->color.g = new_clr.rgb[1];
                      ap->color.b = new_clr.rgb[2];

                      run_light_changer(l_idx);
                      state_mutex.unlock();
                    },
                    std::placeholders::_1));
      break;
    }

    case CHANGER_ARG_BOUNDED_INT: {
      Wt::WContainerWidget *cntr = new Wt::WContainerWidget(group);
      cntr->resize(ColorViewWidget::img_w, Wt::WLength::Auto);

      Wt::WVBoxLayout *layout = new Wt::WVBoxLayout();

      Wt::WSlider *intsld = new Wt::WSlider(Wt::Horizontal);
      layout->addWidget(intsld);

      intsld->setToolTip((boost::format("[%d, %d)") % a.bounded_int.beg %
                          a.bounded_int.end).str());
      intsld->resize(ColorViewWidget::img_w, 50);
      intsld->setRange(a.bounded_int.beg, a.bounded_int.end);
      intsld->setValue(a.bounded_int.beg);
      intsld->valueChanged().connect(std::bind([=](int new_val) {
                                                 state_mutex.lock();
                                                 ap->bounded_int.x = new_val;
                                                 run_light_changer(l_idx);
                                                 state_mutex.unlock();
                                               },
                                               std::placeholders::_1));

      Wt::WText *intsld_lbl =
          new Wt::WText((boost::format("%d") % intsld->value()).str(), group);
      layout->addWidget(intsld_lbl);

      intsld->sliderMoved().connect(std::bind(
          [=](int new_val) {
            intsld_lbl->setText((boost::format("%d") % new_val).str());
          },
          std::placeholders::_1));

      cntr->setLayout(layout);
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
}
}
