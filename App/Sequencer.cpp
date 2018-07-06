#include "Sequencer.h"
#include <poScene/ShapeView.h>

using namespace std::placeholders;


rg::SequencerRef rg::Sequencer::create(int numSteps, float padding)
{
  rg::SequencerRef ref(new Sequencer(numSteps, padding));
  return ref;
}

void rg::Sequencer::viewDidLoad()
{
  po::scene::ViewRef seqView = getView();
  const Theme& theme = Theme::get();

  // add background
  mBackground = po::scene::ShapeView::createRect(1, 1);
  mBackground->setFillColor(theme.backgroundDark);
  mBackground->setSuperviewShouldIgnoreInBounds(true);
  seqView->addSubview(mBackground);
}

void rg::Sequencer::update()
{
  const int N = mPattern.size();
  po::scene::ViewRef seqView = getView();
  const ci::vec2& seqSize = seqView->getSize();
  const ci::vec2& seqUL = seqView->getBounds().getUpperLeft();
  const float padSpacing = (seqSize.x - mPadding) / float(N);
  const float verticalCenter = seqSize.y * 0.5f;

  if (N != mPads.size())
    resetPads(padSpacing - mPadding);

  mBackground->setPosition(seqUL);
  mBackground->setScale(seqSize);

  for (int stepIx = 0; stepIx < N; ++stepIx) {
    SequencerPadRef pad = mPads[stepIx];
    pad->setAlignment(po::scene::Alignment::CENTER_LEFT);
    pad->setPosition(ci::vec2(mPadding + (stepIx * padSpacing), verticalCenter));
    pad->highlight(stepIx == mActiveStep && mPlayback ? SequencerPad::ON : SequencerPad::ON_MOUSE_HOVER);
    bool isNote = mPattern[stepIx];
    pad->setActive(isNote);
  }
}

void rg::Sequencer::setStepCount(int N)
{
  int oldN = getStepCount();
  mPattern.resize(N);
  if (oldN != N)
    sPatternChanged.emit(getPattern());
}

void rg::Sequencer::setPattern(PatternId pattern)
{
  if (pattern == getPattern()) {
    return;
  }

  RhythmPattern::patternId2Vector(pattern, mPattern.begin(), mPattern.end());

  // NOTE: We reconstruct instead of reusing the pattern 
  // parameter because it might have been truncated
  sPatternChanged.emit(getPattern());
}

PatternId rg::Sequencer::getPattern() const
{
  return RhythmPattern::vector2PatternId(mPattern);
}

void rg::Sequencer::setPlayback(bool enabled)
{
  mPlayback = enabled;
}

void rg::Sequencer::setActiveStep(int stepIx)
{
  mActiveStep = stepIx;
}

void rg::Sequencer::setDisabled(bool disabled)
{
  for (SequencerPadRef ref : mPads)
    ref->setDisabled(disabled);
}

rg::Sequencer::Sequencer(int numSteps, float padding) : 
  mPattern(std::vector<bool>(numSteps)),
  mPadding(padding)
{
}

void rg::Sequencer::resetPads(float padWidth)
{
  const int N = mPattern.size();
  po::scene::ViewRef seqView = getView();
  const Theme& theme = Theme::get();
  const ci::vec2 padDimensions(padWidth, seqView->getHeight() - (mPadding * 2.0));

  // clear previous pads
  auto padIt = mPads.cbegin();
  while (padIt != mPads.cend()) {
    seqView->removeSubview(*padIt);
    padIt = mPads.erase(padIt);
  }

  // create new pads
  for (int stepIx = 0; stepIx < N; ++stepIx) {
    SequencerPadRef pad = SequencerPad::create(stepIx, padDimensions);
    pad->setColorBase(theme.backgroundBase);
    pad->setColorActive(theme.secondaryDark);
    pad->setColorBaseHover(theme.backgroundLight);
    pad->setColorActiveHover(theme.secondaryBase);
    pad->sClicked.connect(std::bind(&Sequencer::onPadClicked, this, _1));
    mPads.push_back(pad);
    seqView->addSubview(pad);
  }
}

void rg::Sequencer::onPadClicked(int stepIx)
{
  assert(stepIx <= (int)mPattern.size());
  mPattern[stepIx] = !mPattern[stepIx];
  sPatternChanged.emit(getPattern());
}
