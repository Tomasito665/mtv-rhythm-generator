#pragma once

#include "Sequencer.h"
#include "MTVCanvas.h"
#include "MTVRhythmSpace.h"
#include "PatternPlayer.h"
#include "ProgressBar.h"
#include "KeyboardController.h"
#include <cinder/GeomIo.h>
#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>

using namespace ci;
using namespace ci::app;

class RhythmGenerator : public App
{
public:
  RhythmGenerator();
  virtual ~RhythmGenerator();

  // lifetime events
  void setup() override;
  void update() override;
  void draw() override;
  void cleanup() override;

  // window events
  void resize() override;

  // ui events
  void mouseUp(MouseEvent event) override;
  void mouseDown(MouseEvent event) override;
  void mouseDrag(MouseEvent event) override;
  void keyDown(KeyEvent event) override;
  void keyUp(KeyEvent event) override;

  // tries to create a new mtv rhythm space and fills it asynchronously, returns 
  // false it failed to do so (because current space is busy)
  bool requestNewMtvRhythmSpace(const TimeSignature& ts, UnitRef stepUnit);

  void togglePatternPlayback(); // starts or stops rhythm pattern playback
  void togglePatternLoopEnabled(); // enabled or disabled looped pattern playback

  // app-specific events
  void patternChange(const RhythmPattern& pattern);
  void closestPatternRequest();
  void randomClosePatternRequest();

  // setup
  void setupLayout();
  void setupSignals();

protected:
  enum AsyncAction { MTV_RHYTHM_SPACE_FILL };
  void asyncAction(AsyncAction action);
  void checkFutures();

private:
  std::mutex mMutex;
  std::unordered_map<AsyncAction, std::future<void>> mFutures;

  PatternPlayer mPlayer;
  Sequencer mSequencer;
  MTVCanvas mMtvCanvas;
  KeyboardController mKbdController;
  MTVRhythmSpace * mMtvRhythmSpace;
};

CINDER_APP( RhythmGenerator, RendererGl )
