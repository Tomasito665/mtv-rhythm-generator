#pragma once

#include <cinder/app/App.h>
#include <cinder/Signals.h>
#include <poScene/View.h>
#include "ToggleButton.h"

namespace rg {
  typedef std::shared_ptr<class SequencerPad> SequencerPadRef;

  class SequencerPad : public ToggleButton
  {
  public:
    static SequencerPadRef create(int id, ci::vec2 dimensions = ci::vec2(50, 50));
    ci::signals::Signal<void(int id)> sClicked;

  protected:
    po::scene::ShapeViewRef createToggleShape() override;
    void onClick() override;

  private:
    SequencerPad(int id, ci::vec2 dimensions);

    int mId;
    ci::vec2 mDimensions;
  };
}
