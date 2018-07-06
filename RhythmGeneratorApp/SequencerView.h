#pragma once

#include "RhythmPattern.h"
#include <poScene/View.h>


class SequencerView;
typedef std::shared_ptr<SequencerView> SequencerViewRef;


class SequencerView : public po::scene::View
{
public:
  static SequencerViewRef create(RhythmPatternRef pattern = nullptr);
  virtual ~SequencerView();

  void update() override;
  void draw() override;

private:
  explicit SequencerView(RhythmPatternRef pattern);
  RhythmPatternRef mPattern;
};
