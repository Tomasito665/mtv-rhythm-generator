#include "SequencerPad.h"
#include <poScene/ShapeView.h>

using namespace std::placeholders;


rg::SequencerPadRef rg::SequencerPad::create(int id, ci::vec2 dimensions)
{
  SequencerPadRef ref(new SequencerPad(id, dimensions));
  ref->setup();
  return ref;
}

po::scene::ShapeViewRef rg::SequencerPad::createToggleShape()
{
  const ci::vec2& d = mDimensions;
  return po::scene::ShapeView::createRect(d.x, d.y, std::min(d.x, d.y) * 0.28f);
}

void rg::SequencerPad::onClick()
{
  sClicked.emit(mId);
}

rg::SequencerPad::SequencerPad(int id, ci::vec2 dimensions) :
  mId(id),
  mDimensions(dimensions)
{
}
