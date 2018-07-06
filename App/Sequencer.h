#pragma once

#include <poScene/ViewController.h>
#include <cinder/Signals.h>
#include "RhythmPattern.h"
#include "SequencerPad.h"
#include "Theme.h"

namespace rg {
  typedef std::shared_ptr<class Sequencer> SequencerRef;

  class Sequencer : public po::scene::ViewController
  {
  public:
    static SequencerRef create(int numSteps = 16, float padding = 16.0f);

    void viewDidLoad() override;
    void update() override;

    void setStepCount(int numSteps);
    int getStepCount() const { return mPattern.size(); }

    void setPattern(PatternId pattern);
    PatternId getPattern() const;

    void setPlayback(bool enabled);
    inline bool getPlayback() const { return mPlayback; }
    void setActiveStep(int stepIx);
    inline int getActiveStep() const { return mActiveStep; }

    void setDisabled(bool disabled);

    ci::signals::Signal<void(PatternId)> sPatternChanged;

  protected:
    explicit Sequencer(int numSteps, float padding);
    void resetPads(float padWidth);
    void onPadClicked(int padId);

  private:
    po::scene::ShapeViewRef mBackground;
    std::vector<rg::SequencerPadRef> mPads;
    std::vector<bool> mPattern;
    float mPadding;
    bool mPlayback;
    int mActiveStep;
  };
}
