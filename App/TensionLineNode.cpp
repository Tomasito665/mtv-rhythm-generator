#include "TensionLineNode.h"
#include <poScene/ShapeView.h>

using namespace std::placeholders;


rg::TensionLineNodeRef rg::TensionLineNode::create(int id, float radius, ci::Color baseColor, ci::Color blimpColor)
{
  rg::TensionLineNodeRef ref(new TensionLineNode(id, radius));
  ref->setBaseColor(baseColor);
  ref->setBlimpColor(blimpColor);
  ref->setup();
  return ref;
}

void rg::TensionLineNode::setup()
{
  mCircle = po::scene::ShapeView::createCircle(mRadius * 2.0f);
  mCircle->setAlignment(po::scene::Alignment::CENTER_CENTER);
  mCircle->setFillColor(mBaseColor);
  addSubview(mCircle);
}

void rg::TensionLineNode::update()
{
}

void rg::TensionLineNode::blimp()
{
  mCircle->setFillColor(mBlimpColor);
  ci::app::timeline().apply(&mCircle->getFillColorAnim(), mBaseColor, 1.0f, ci::EaseOutAtan());
}

void rg::TensionLineNode::setBaseColor(const ci::Color& color)
{
  // don't set fill color if not necessary not to cancel the blimp animation
  if (mBaseColor != color && mCircle) {
    mCircle->setFillColor(color);
  }

  mBaseColor = color;
}

void rg::TensionLineNode::setBlimpColor(const ci::Color& color)
{
  mBlimpColor = color;
}

rg::TensionLineNode::TensionLineNode(int id, float radius) :
  mId(id),
  mRadius(radius)
{
}
