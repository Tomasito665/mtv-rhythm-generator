#include "Sequencer.h"
#include "Types.h"
#include <cinder/gl/gl.h>
#include <algorithm>
#include <stdexcept>
#include <assert.h>

Sequencer::Sequencer(const TimeSignature & ts, UnitRef stepUnit, const Rectf & rect, int stepRectMargin) :
  mPattern(RhythmPattern(ts, stepUnit)),
  mActiveStepIx(-1),
  mMainRect(rect),
  mStepRectMargin(stepRectMargin)
{
  reallocateStepRects();
}

Sequencer::~Sequencer()
{
}

void Sequencer::setTimeSignature(const TimeSignature& ts)
{
  mPattern.reset(ts);
  reallocateStepRects();
}

void Sequencer::setStepUnit(UnitRef unit)
{
  mPattern.reset(unit);
  reallocateStepRects();
}

void Sequencer::setTimeSignatureAndStepUnit(const TimeSignature& ts, UnitRef stepUnit)
{
  mPattern.reset(ts, stepUnit);
  reallocateStepRects();
}

void Sequencer::setPattern(const RhythmPattern& pattern)
{
  const int prevNSteps = mPattern.getNSteps();
  const int currNSteps = pattern.getNSteps();
  mPattern = pattern;
  
  if (currNSteps != prevNSteps)
    reallocateStepRects();

  sPatternChanged.emit(mPattern);
}

void Sequencer::setPatternById(PatternId pattern)
{
  mPattern.update(pattern);
  sPatternChanged.emit(mPattern);
}

void Sequencer::setRect(const Rectf & rect)
{
  mMainRect = rect;
  updateStepRectPositions();
}

void Sequencer::draw()
{
  int nSteps = getNSteps();

  gl::lineWidth(2);
  gl::color(Color(1, 1, 1));
  gl::drawStrokedRect(mMainRect);

  for (int i = 0; i < nSteps; ++i) {
    const Rectf& rect = mStepRects[i];
    if (mPattern[i])
      gl::drawSolidRect(rect);
    else
      gl::drawStrokedRect(rect);
  }
}

void Sequencer::mouseDown(const MouseEvent& event)
{
  activeMouseOnSequencer(event.getPos());
}

void Sequencer::mouseDrag(const MouseEvent& event)
{
  activeMouseOnSequencer(event.getPos());
}

void Sequencer::mouseUp(const MouseEvent& event)
{
  mActiveStepIx = -1;
}

int Sequencer::pos2StepIx(int posX)
{
  const vec2& mainRectUL = mMainRect.getUpperLeft();
  const float mainRectWidth = mMainRect.getWidth();
  // convert to local x position and clamp to [0, width]
  float localPosX = std::max(.0f, std::min(posX - mainRectUL.x, mainRectWidth));
  return int(localPosX / mainRectWidth * getNSteps());
}

void Sequencer::activeMouseOnSequencer(vec2 pos)
{
  if (!mMainRect.contains(pos)) {
    mActiveStepIx = -1;
    return;
  }

  const int stepIx = pos2StepIx((int)pos.x);
  const Rectf& stepRect = mStepRects[stepIx];

  if (!stepRect.contains(pos)) {
    return;
  }

  if (mActiveStepIx != stepIx) {
    mPattern.toggle(stepIx);
    sPatternChanged.emit(mPattern);
  }

  mActiveStepIx = stepIx;
}

void Sequencer::reallocateStepRects()
{
  const int nSteps = mPattern.getNSteps();
  if (mStepRects) delete[] mStepRects;
  mStepRects = new Rectf[nSteps];
  mPattern.reset();
  updateStepRectPositions();
}

void Sequencer::updateStepRectPositions()
{
  const int nSteps = getNSteps();
  const vec2& seqPos = mMainRect.getUpperLeft();
  const float seqWidth = mMainRect.getWidth();
  const float seqHeight = mMainRect.getHeight();
  const float stepWidth = seqWidth / (float)nSteps - mStepRectMargin;
  const float stepHeight = seqHeight - mStepRectMargin;
  vec2 stepRectCenterPos(0, seqPos.y + seqHeight / 2);

  for (int i = 0; i < nSteps; ++i) {
    // compute the center x position of this step rect
    stepRectCenterPos.x = (i + .5f) / nSteps * seqWidth + seqPos.x;

    // update rect position
    Rectf& rect = mStepRects[i];
    rect.set(0, 0, stepWidth, stepHeight);
    rect.offsetCenterTo(stepRectCenterPos);
  }
}
