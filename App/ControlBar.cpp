#include "ControlBar.h"
#include <poScene/ShapeView.h>
#include "Theme.h"

using namespace std::placeholders;


rg::ControlBarRef rg::ControlBar::create()
{
  return ControlBarRef(new ControlBar);
}

void rg::ControlBar::viewDidLoad()
{
  po::scene::ViewRef view = getView();
  const Theme& theme = Theme::get();

  // add background
  mBackground = po::scene::ShapeView::createRect(1, 1);
  mBackground->setFillColor(theme.secondaryDark);
  mBackground->setSuperviewShouldIgnoreInBounds(true);
  view->addSubview(mBackground);

  // add time signature multi-switch  
  mTimeSignatureSwitch = TimeSignatureSwitchRef(new MultiSwitch<TimeSignature*>("Meter", 64.0f));
  mTimeSignatureSwitch->setup();
  mTimeSignatureSwitch->addOption("4/4", &mTimeSignatures[0]);
  mTimeSignatureSwitch->addOption("3/4", &mTimeSignatures[1]);
  mTimeSignatureSwitch->addOption("6/8", &mTimeSignatures[2]);
  mTimeSignatureSwitch->sOptionClicked.connect(std::bind(
    &ControlBar::onTimeSignatureOptionClicked, this, _1, _2
  ));

  // add step unit multi-switch
  mStepUnitSwitch = StepUnitSwitchRef(new MultiSwitch<UnitRef>("Step", 64.0f));
  mStepUnitSwitch->setup();
  mStepUnitSwitch->addOption("1/16", mStepUnits[0]);
  mStepUnitSwitch->addOption("1/8", mStepUnits[1]);
  mStepUnitSwitch->addOption("1/4", mStepUnits[2]);
  mStepUnitSwitch->sOptionClicked.connect(std::bind(
    &ControlBar::onStepUnitOptionClicked, this, _1, _2
  ));

  view->addSubviews({ mBackground, mTimeSignatureSwitch, mStepUnitSwitch });
}

void rg::ControlBar::update()
{
  po::scene::ViewRef view = getView();
  const ci::Rectf& bounds = view->getBounds();
  const ci::vec2& size = view->getSize();
  const ci::vec2& pos = bounds.getUpperLeft();
  const float centerY = size.y * 0.5f;

  mBackground->setPosition(pos);
  mBackground->setScale(size);

  // align multi-switch views in a horizontal row
  mTimeSignatureSwitch->setPosition(0, centerY);
  mStepUnitSwitch->setPosition(mTimeSignatureSwitch->getWidth() + 25.0f, centerY);
}

bool rg::ControlBar::setSelectedTimeSignature(const TimeSignature& ts)
{
  int ix = 0;
  for (const TimeSignature& currTs : mTimeSignatures) {
    if (ts == currTs) {
      mTimeSignatureSwitch->setByIndex(ix);
      return true;
    }
    ++ix;
  }
  return false;
}

bool rg::ControlBar::setSelectedStepUnit(UnitRef unit)
{
  int ix = 0;
  for (UnitRef currUnit : mStepUnits) {
    if (*(currUnit) == *(unit)) {
      mStepUnitSwitch->setByIndex(ix);
      return true;
    }
    ++ix;
  }
  return false;
}

TimeSignature rg::ControlBar::getSelectedTimeSignature() const
{
  return *mTimeSignatureSwitch->get();
}

UnitRef rg::ControlBar::getSelectedStepUnit() const
{
  return mStepUnitSwitch->get();
}

void rg::ControlBar::setDisabled(bool disabled)
{
  mTimeSignatureSwitch->setDisabled(disabled);
  mStepUnitSwitch->setDisabled(disabled);
}

void rg::ControlBar::onTimeSignatureOptionClicked(int optIx, TimeSignature * ts)
{
  sTimeSignatureRequest.emit(*ts);
}

void rg::ControlBar::onStepUnitOptionClicked(int optIx, UnitRef unit)
{
  sStepUnitRequest.emit(unit);
}

rg::ControlBar::ControlBar()
{

}
