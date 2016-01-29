#pragma once
#include <Wt/WContainerWidget>

class SquidLightsWidget : public Wt::WContainerWidget
{
public:
  SquidLightsWidget();

#if 0
private:
  Wt::WNavigationBar *navigation_;
  Wt::WStackedWidget *contentsStack_;

  Wt::WMenuItem *addToMenu(Wt::WMenu *menu,
			   const Wt::WString& name,
			   TopicWidget *topic);
#endif
};
