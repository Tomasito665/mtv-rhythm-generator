#include "SequencerView.h"



SequencerViewRef SequencerView::create(RhythmPatternRef pattern)
{
  SequencerViewRef ref(new SequencerView(pattern));

  return SequencerViewRef();
}

SequencerView::SequencerView(RhythmPatternRef pattern) : 
  mPattern(pattern)
{
}

SequencerView::~SequencerView()
{
}

void SequencerView::update()
{
}

void SequencerView::draw()
{

}
