#include "ToggleButton.h"
#include <poScene/ShapeView.h>
#include "Utils.h"

using po::scene::MouseEvent;
using namespace std::placeholders;

void rg::ToggleButton::setup()
{
  highlight(ON_MOUSE_HOVER);
  mShape = createToggleShape();
  mShape->getSignal(MouseEvent::MOVE).connect(std::bind(&ToggleButton::onMouseMove, this, _1));
  mShape->getSignal(MouseEvent::DOWN_INSIDE).connect(std::bind(&ToggleButton::onMouseDownInside, this, _1));
  mShape->getSignal(MouseEvent::UP).connect(std::bind(&ToggleButton::onMouseUp, this, _1));
  addSubview(mShape);
}

void rg::ToggleButton::update()
{
  if (mDisabled) return;
  bool highlight = false;

  switch (mHighlight)
  {
  case rg::ToggleButton::ON_MOUSE_HOVER:
    highlight = mMouseHover;
    break;
  case rg::ToggleButton::ON:
    highlight = true;
    break;
  case rg::ToggleButton::OFF:
    highlight = false;
    break;
  default:
    break;
  }

  if (highlight)
    mShape->setFillColor(mActive ? mColorActiveHover : mColorBaseHover);
  else
    mShape->setFillColor(mActive ? mColorActive : mColorBase);
}

void rg::ToggleButton::setActive(bool active)
{
  mActive = active;
}

void rg::ToggleButton::highlight(Highlight highlight)
{
  mHighlight = highlight;
}

void rg::ToggleButton::setDisabled(bool disabled)
{
  if (mDisabled = disabled) {
    mShape->setFillColor(mActive 
      ? toGrayscale(mColorActive) 
      : toGrayscale(mColorBase));
  }
}

rg::ToggleButton::ToggleButton()
{
}

void rg::ToggleButton::onMouseMove(MouseEvent& event)
{
  ci::vec2 ePosWin = event.getWindowPos();
  ci::Rectf squareBoundsScene = mShape->getBounds();
  ci::Rectf squareBoundsWin(
    mShape->localToWindow(squareBoundsScene.getUpperLeft()),
    mShape->localToWindow(squareBoundsScene.getLowerRight())
  );
  mMouseHover = squareBoundsWin.contains(ePosWin);
}

void rg::ToggleButton::onMouseDownInside(MouseEvent& event)
{
  if (!mMouseDown && !mDisabled) { onClick(); }
  mMouseDown = true;
}

void rg::ToggleButton::onMouseUp(MouseEvent& event)
{
  mMouseDown = false;
}
