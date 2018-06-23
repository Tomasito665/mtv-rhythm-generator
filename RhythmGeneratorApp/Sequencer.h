#pragma once

#include "Types.h"
#include "RhythmPattern.h"
#include <cinder/app/App.h>
#include <vector>
#include <bitset>

using namespace ci;
using namespace ci::app;


class Sequencer
{
public:
  Sequencer(
    const TimeSignature& ts = TimeSignature(4, 4),
    UnitRef stepUnit = Unit::QUAVER,
    const Rectf& rect = Rectf(0, 0, 350, 50), 
    int stepRectMargin = 10
  );

  virtual ~Sequencer();

  // sets time signature, updates step count and resets rhythm pattern
  void setTimeSignature(const TimeSignature& ts);
  // sets step unit, updates step count and resets rhythm pattern
  void setStepUnit(UnitRef unit);
  // sets both the time signature and the step unit (this is more efficient 
  // than using separate setters one after the other)
  void setTimeSignatureAndStepUnit(const TimeSignature& ts, UnitRef stepUnit);
  // sets the pattern
  void setPattern(const RhythmPattern& pattern);
  // sets the pattern by its id
  void setPatternById(PatternId pattern);

  void setRect(const Rectf& rect);
  void draw();

  // supported events
  void mouseDown(const MouseEvent& event);
  void mouseDrag(const MouseEvent& event);
  void mouseUp(const MouseEvent& event);

  // signals
  signals::Signal<void(const RhythmPattern& pattern)> sPatternChanged;

  // getters
  inline const TimeSignature& getTimeSignature() const { return mPattern.getTimeSignature(); }
  inline const UnitRef getStepUnit() const { return mPattern.getStepUnit(); }
  inline int getNSteps() { return mPattern.getNSteps(); }
  inline RhythmPattern const & getPattern() const { return mPattern; }
  inline PatternId getPatternId() const { return mPattern.getPatternId(); }

protected:
  int pos2StepIx(int posX);
  void activeMouseOnSequencer(vec2 pos);
  void reallocateStepRects();
  void updateStepRectPositions();

private:
  RhythmPattern mPattern;
  bool mIsMouseDown;
  int mActiveStepIx;
  Rectf mMainRect;
  Rectf * mStepRects;
  int mStepRectMargin;
};
