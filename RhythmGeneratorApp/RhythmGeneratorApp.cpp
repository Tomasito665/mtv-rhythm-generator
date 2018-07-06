#include "RhythmGeneratorApp.h"
#include "MTVCanvas.h"
#include <cinder/Log.h>

using namespace std::placeholders;
using namespace ci::log;


//RhythmGenerator::RhythmGenerator()
//{
//  TimeSignature timesig(4, 4);
//  UnitRef stepUnit = Unit::SEMIQUAVER;
//
//  mSequencer.setTimeSignatureAndStepUnit(timesig, stepUnit);
//  mMtvCanvas.reset(mSequencer.getNSteps());
//}
//
//RhythmGenerator::~RhythmGenerator()
//{
//  if (mMtvRhythmSpace)
//    delete mMtvRhythmSpace;
//}
//
//void RhythmGenerator::setup()
//{
//  CI_LOG_D("RhythmGenerator::setup()");
//  setupSignals();
//  setupLayout();
//  mPlayer.setup();
//  mPlayer.startPolling();
//  requestNewMtvRhythmSpace(
//    mSequencer.getTimeSignature(), 
//    mSequencer.getStepUnit());
//}
//
//void RhythmGenerator::update()
//{
//  checkFutures();
//  mMtvCanvas.update();
//}
//
//void RhythmGenerator::draw()
//{
//  gl::clear();
//  mMtvCanvas.draw();
//  mSequencer.draw();
//}
//
//void RhythmGenerator::cleanup()
//{
//  mPlayer.stopPolling(true);
//
//  // When cleanup is called, setup() won't be called anymore, and 
//  // consequently, neither will checkFutures(). This allows us to 
//  // iterate over all pending futures and wait for each.
//  for (const auto& actionFuturePair : mFutures) {
//    const std::future<void>& future = actionFuturePair.second;
//    future.wait();
//  }
//}
//
//void RhythmGenerator::resize()
//{
//  setupLayout();
//}
//
//void RhythmGenerator::mouseUp(MouseEvent event)
//{
//  mMtvCanvas.mouseUp(event);
//  mSequencer.mouseUp(event);
//}
//
//void RhythmGenerator::mouseDown(MouseEvent event)
//{
//  mMtvCanvas.mouseDown(event);
//  mSequencer.mouseDown(event);
//}
//
//void RhythmGenerator::mouseDrag(MouseEvent event)
//{
//  mMtvCanvas.mouseDrag(event);
//  mSequencer.mouseDrag(event);
//}
//
//void RhythmGenerator::keyDown(KeyEvent event)
//{
//  mKbdController.keyDown(event);
//}
//
//void RhythmGenerator::keyUp(KeyEvent event)
//{
//  mKbdController.keyUp(event);
//}
//
//void RhythmGenerator::setupLayout()
//{
//  const int windowWidth = getWindowWidth();
//  const int windowHeight = getWindowHeight();
//
//  const float margin = 50;
//  const float sequencerHeight = 50;
//  const float containerWidth = windowWidth * 0.85f;
//  const float mtvCanvasHeight = windowHeight * 0.50f;
//  const vec2 windowCenter(getWindowWidth() / 2.0f, getWindowHeight() / 2.0f);
//
//  Rectf mtvCanvasRect(0, 0, containerWidth, mtvCanvasHeight);
//  Rectf sequencerRect(0, 0, containerWidth, sequencerHeight);
//  sequencerRect.moveULTo(mtvCanvasRect.getLowerLeft() + vec2(0, margin));
//
//  // calculate necessary offset to center everything as a whole
//  Rectf containerRect(mtvCanvasRect.getUpperLeft(), sequencerRect.getLowerRight());
//  const vec2 centerOffset(vec2(
//    windowCenter.x - containerRect.getWidth() / 2.0,
//    windowCenter.y - containerRect.getHeight() / 2.0
//  ) - containerRect.getUpperLeft());
//
//  // center everything
//  mtvCanvasRect += centerOffset;
//  sequencerRect += centerOffset;
//
//  // setup locations
//  mMtvCanvas.setRect(mtvCanvasRect);
//  mSequencer.setRect(sequencerRect);
//}
//
//void RhythmGenerator::setupSignals()
//{
//  mSequencer.sPatternChanged.connect(std::bind(&RhythmGenerator::patternChange, this, _1));
//  // keyboard bindings
//  mKbdController.bind(KeyEvent::KEY_g, std::bind(&RhythmGenerator::randomClosePatternRequest, this));
//  mKbdController.bind(KeyEvent::KEY_c, std::bind(&RhythmGenerator::closestPatternRequest, this));
//  mKbdController.bind(KeyEvent::KEY_SPACE, std::bind(&RhythmGenerator::togglePatternPlayback, this));
//  mKbdController.bind(KeyEvent::KEY_l, std::bind(&RhythmGenerator::togglePatternLoopEnabled, this));
//}
//
//void RhythmGenerator::asyncAction(AsyncAction action)
//{
//  switch (action)
//  {
//  case RhythmGenerator::MTV_RHYTHM_SPACE_FILL:
//    mMtvCanvas.showProgressBar(false);
//    break;
//  default:
//    break;
//  }
//}
//
//void RhythmGenerator::checkFutures()
//{
//  std::lock_guard<std::mutex> lock(mMutex);
//  std::unordered_map<AsyncAction, std::future<void>>::const_iterator it = mFutures.cbegin();
//
//  while (it != mFutures.cend()) {
//    const AsyncAction action = it->first;
//    const std::future<void>& future = it->second;
//    std::future_status status = it->second.wait_for(std::chrono::seconds(0));
//
//    if (status == std::future_status::ready) {
//      asyncAction(action);
//      it = mFutures.erase(it);
//      continue;
//    }
//
//    // shouldn't defer since we use async launch policy
//    assert(status == std::future_status::timeout);
//
//    ++it;
//  }
//}
//
//void RhythmGenerator::patternChange(const RhythmPattern& pattern)
//{
//  CI_LOG_D("RhythmGenerator::patternChange()");
//
//  if (pattern != mSequencer.getPattern())
//    mSequencer.setPattern(pattern);
//
//  if (pattern != mPlayer.getPattern())
//    mPlayer.setPattern(pattern);
//
//  if (mMtvRhythmSpace && mMtvRhythmSpace->ready()) {
//    const Tension * const mtv = mMtvRhythmSpace->getMTV(pattern.getPatternId());
//    mMtvCanvas.updateLockedMtv(mtv);
//  }
//}
//
//void RhythmGenerator::closestPatternRequest()
//{
//  if (!mMtvRhythmSpace) {
//    mSequencer.setPatternById(EMPTY_RHYTHM_PATTERN);
//    return;
//  }
//
//  const Tension * const targetMtv = mMtvCanvas.getFreeMtv();
//  PatternId pattern = mMtvRhythmSpace->getClosestPattern(targetMtv);
//  mSequencer.setPatternById(pattern);
//}
//
//void RhythmGenerator::randomClosePatternRequest()
//{
//  if (!mMtvRhythmSpace) {
//    mSequencer.setPatternById(EMPTY_RHYTHM_PATTERN);
//    return;
//  }
//
//  const Tension * const targetMtv = mMtvCanvas.getFreeMtv();
//  PatternId pattern = mMtvRhythmSpace->getRandomPatternCloseTo(targetMtv);
//  mSequencer.setPatternById(pattern);
//}
//
//bool RhythmGenerator::requestNewMtvRhythmSpace(const TimeSignature& ts, UnitRef stepUnit)
//{
//  std::lock_guard<std::mutex> lock(mMutex);
//
//  // if there's already a pending MTV_RHYTHM_SPACE_FILL
//  if (mFutures.find(AsyncAction::MTV_RHYTHM_SPACE_FILL) != mFutures.end()) {
//    return false;
//  }
//
//  if (mMtvRhythmSpace) delete mMtvRhythmSpace;
//  mMtvRhythmSpace = new MTVRhythmSpace(ts, stepUnit);
//
//  mMtvCanvas.setProgressBarValue(0.0);
//  mMtvCanvas.showProgressBar(true);
//
//  std::function<void(double)> setProgress = std::bind(&MTVCanvas::setProgressBarValue, &mMtvCanvas, _1);
//  std::function<void()> fillSpace = std::bind(&MTVRhythmSpace::fill, mMtvRhythmSpace, setProgress);
//  mFutures[AsyncAction::MTV_RHYTHM_SPACE_FILL] = std::async(std::launch::async, fillSpace);  
//
//  return true;
//}
//
//void RhythmGenerator::togglePatternPlayback()
//{
//  if (mPlayer.isPlaying())
//    mPlayer.stopPlayback();
//  else
//    mPlayer.startPlayback();
//}
//
//void RhythmGenerator::togglePatternLoopEnabled()
//{
//  if (mPlayer.isLoopEnabled())
//    mPlayer.enableLoop(false);
//  else
//    mPlayer.enableLoop(true);
//}

RhythmGeneratorApp::RhythmGeneratorApp()
{
}

RhythmGeneratorApp::~RhythmGeneratorApp()
{
}

void RhythmGeneratorApp::setup()
{
  mViewCtrl = ViewController::create();
  mScene = po::scene::Scene::create(mViewCtrl);
}

void RhythmGeneratorApp::update()
{
  mScene->update();
}

void RhythmGeneratorApp::draw()
{
  mScene->draw();
}

void RhythmGeneratorApp::cleanup()
{
  mViewCtrl->cleanup();
}
