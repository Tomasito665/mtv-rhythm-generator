#include "App.h"
#include "MTVRhythmSpace.h"
#include <mutex>

using namespace std::placeholders;


void rg::App::prepareSettings(Settings * settings)
{
  settings->setWindowSize(1920, 1080);
  settings->setTitle("Rhythm Generator");
}

void rg::App::setup()
{
  // NOTE: Must go before view controller creation as it uses 
  // the singleton theme which is set up in setupTheme()
  setupTheme();

  mViewCtrl = rg::MainViewController::create();
  mScene = po::scene::Scene::create(mViewCtrl);

  mPatternPlayer.setup();
  mPatternPlayer.startPolling();
  mPatternPlayer.setLoopEnabled(true);
  mPatternPlayer.setClick(true);
  mPatternPlayer.setBpm(90);

  setupSignals();
  resetMtvRhythmSpace(TimeSignature(4, 4), Unit::QUAVER);
}

void rg::App::update()
{
  checkFutures();
  mScene->update();
}

void rg::App::draw()
{
  ci::gl::clear(ci::Color(0, 0, 0));
  mScene->draw();
}

void rg::App::cleanup()
{
  mPatternPlayer.stopPolling(true);
  std::lock_guard<std::mutex> lock(mMutex);
  if (mFutureGetRandomPatternCloseToMtv.valid())
    mFutureGetRandomPatternCloseToMtv.wait();
  if (mFutureRhythmSpaceReset.valid())
    mFutureRhythmSpaceReset.wait();
}

void rg::App::keyUp(ci::app::KeyEvent event)
{
  mKbdController.keyUp(event);
}

void rg::App::keyDown(ci::app::KeyEvent event)
{
  mKbdController.keyDown(event);
}

void rg::App::resetMtvRhythmSpace(const TimeSignature& ts, UnitRef unit)
{
  std::lock_guard<std::mutex> lock(mMutex);

  // fail if there's already a pending request for a mtv rhythm space fill
  if ((mMtvRhythmSpace && !mMtvRhythmSpace->ready()) || mFutureRhythmSpaceReset.valid()) {
    onMtvRhythmSpaceReset(true);
    return;
  }

  // delete the previous space and create new one
  if (mMtvRhythmSpace) delete mMtvRhythmSpace;
  mMtvRhythmSpace = new MTVRhythmSpace(ts, unit);

  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  ControlBarRef controlBar = mViewCtrl->getControlBar();
  RhythmPatternPlayer& player = mPatternPlayer;

  if (canvas) {
    canvas->setLoadingProgress(0.0);
    canvas->setLoading(true);
  }

  if (controlBar) {
    controlBar->setDisabled(true);
  }

  player.setBeatDuration(ts.getBeatUnit()->convertExact(1, unit));

  std::function<void(double)> updateProgress = std::bind(
    &TensionCanvas::setLoadingProgress, canvas.get(), _1
  );

  mFutureRhythmSpaceReset = std::async(std::launch::async, std::bind(
    &MTVRhythmSpace::fill, mMtvRhythmSpace, updateProgress));
}

void rg::App::setPatternClosestToMtv()
{
  MTVRhythmSpace * space = mMtvRhythmSpace;
  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  SequencerRef sequencer = mViewCtrl->getSequencer();

  if (!space || !sequencer) {
    return;
  }

  const Tension * const mtv = canvas->getFreeTensionLine();
  PatternId pattern = space->getClosestPattern(mtv);
  sequencer->setPattern(pattern);
}

void rg::App::setRandomPatternCloseToMtv()
{
  std::lock_guard<std::mutex> lock(mMutex);

  MTVRhythmSpace * space = mMtvRhythmSpace;
  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  SequencerRef sequencer = mViewCtrl->getSequencer();

  if (!space || !sequencer || !space->ready() || mFutureGetRandomPatternCloseToMtv.valid()) {
    onGetRandomPatternCloseToMtv(0, true);
    return;
  }

  ControlBarRef controlBar = mViewCtrl->getControlBar();
  if (controlBar) controlBar->setDisabled(true);
  sequencer->setDisabled(true);

  const Tension * const mtv = canvas->getFreeTensionLine();
  mFutureGetRandomPatternCloseToMtv = std::async(std::launch::async, std::bind(
    &MTVRhythmSpace::getRandomPatternCloseTo, mMtvRhythmSpace, mtv, 0.1f));
}

void rg::App::toggleRhythmPatternPlayerPlayback()
{
  if (mPatternPlayer.isPlaying())
    mPatternPlayer.stopPlayback();
  else
    mPatternPlayer.startPlayback();
}

void rg::App::toggleRhythmPatternPlayerLoop()
{
  mPatternPlayer.setLoopEnabled(!mPatternPlayer.isLoopEnabled());
}

void rg::App::toggleRhythmPatternPlayerClick()
{
  mPatternPlayer.setClick(!mPatternPlayer.getClick());
}

void rg::App::checkFutures()
{
  std::lock_guard<std::mutex> lock(mMutex);

  if (mFutureRhythmSpaceReset.valid()) {
    std::future_status status = mFutureRhythmSpaceReset.wait_for(std::chrono::seconds(0));
    if (status == std::future_status::ready) {
      onMtvRhythmSpaceReset();
      mFutureRhythmSpaceReset = std::shared_future<void>();
    }
  }

  if (mFutureGetRandomPatternCloseToMtv.valid()) {
    std::future_status status = mFutureGetRandomPatternCloseToMtv.wait_for(std::chrono::seconds(0));
    if (status == std::future_status::ready) {
      PatternId pattern = mFutureGetRandomPatternCloseToMtv.get();
      onGetRandomPatternCloseToMtv(pattern);
      mFutureGetRandomPatternCloseToMtv = std::shared_future<PatternId>();
    }
  }
}

void rg::App::setupTheme()
{
  Theme& theme = Theme::get();
  theme.backgroundLight = ci::Color::hex(0x588188);
  theme.backgroundBase = ci::Color::hex(0x1A535C);
  theme.backgroundDark = ci::Color::hex(0x0C262A);
  theme.primaryDark = ci::Color::hex(0x306688);
  theme.primaryBase = ci::Color::hex(0x3A7CA5);
  theme.primaryLight = ci::Color::hex(0x5D93B5);
  theme.secondaryDark = ci::Color::hex(0x9FCC5C);
  theme.secondaryBase = ci::Color::hex(0xC2F970);
  theme.secondaryLight = ci::Color::hex(0xCDFA8A);
  theme.grayBase = ci::Color::hex(0x616161);
  theme.grayDark = ci::Color::hex(0x212121);
  theme.accent = ci::Color::hex(0xF9DC5C);
}

void rg::App::setupSignals()
{
  SequencerRef sequencer = mViewCtrl->getSequencer();
  ControlBarRef controlBar = mViewCtrl->getControlBar();
  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  RhythmPatternPlayer& player = mPatternPlayer;
  assert(sequencer && controlBar && canvas);

  canvas->sTensionLineChanged.connect(std::bind(&App::onTensionLineChanged, this, _1, _2, _3));
  sequencer->sPatternChanged.connect(std::bind(&App::onPatternChanged, this, _1));
  player.sPlayback.connect(std::bind(&Sequencer::setPlayback, sequencer, _1));
  player.sStep.connect(std::bind(&Sequencer::setActiveStep, sequencer, _1));
  controlBar->sTimeSignatureRequest.connect(std::bind(&App::onTimeSignatureRequest, this, _1));
  controlBar->sStepUnitRequest.connect(std::bind(&App::onStepUnitRequest, this, _1));
  mKbdController.bind(KeyEvent::KEY_g, std::bind(&App::setRandomPatternCloseToMtv, this));
  mKbdController.bind(KeyEvent::KEY_c, std::bind(&App::setPatternClosestToMtv, this));
  mKbdController.bind(KeyEvent::KEY_SPACE, std::bind(&App::toggleRhythmPatternPlayerPlayback, this));
  mKbdController.bind(KeyEvent::KEY_l, std::bind(&App::toggleRhythmPatternPlayerLoop, this));
  mKbdController.bind(KeyEvent::KEY_c, std::bind(&App::toggleRhythmPatternPlayerClick, this));
}

void rg::App::onTensionLineChanged(const Tension * const freeMtv, const Tension * const lockedMtv, int nSteps)
{
  MTVRhythmSpace * space = mMtvRhythmSpace;
  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  
  if (!canvas) {
    return;
  }

  if (!space || space->getDimensions() != nSteps) {
    canvas->setMtvDistance(-1);
    return;
  }

  const float distance = space->getDistance(freeMtv, lockedMtv);
  canvas->setMtvDistance(distance);
}

void rg::App::onMtvRhythmSpaceReset(bool error)
{
  MTVRhythmSpace * space = mMtvRhythmSpace;
  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  SequencerRef sequencer = mViewCtrl->getSequencer();
  ControlBarRef controlBar = mViewCtrl->getControlBar();

  canvas->setLoading(false);

  if (error) {
    return;
  }

  assert(space);
  const int N = space->getDimensions();

  if (sequencer) {
    sequencer->setStepCount(N);
  }

  if (canvas) { 
    canvas->setStepCount(N);

    if (sequencer) {
      PatternId pattern = sequencer->getPattern();
      canvas->setLockedTensionLine(space->getMTV(pattern));
    }
  }

  if (controlBar) {
    controlBar->setSelectedStepUnit(space->getStepUnit());
    controlBar->setSelectedTimeSignature(space->getTimeSignature());
    controlBar->setDisabled(false);
  }
}

void rg::App::onGetRandomPatternCloseToMtv(PatternId pattern, bool error)
{
  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  ControlBarRef controlBar = mViewCtrl->getControlBar();
  SequencerRef sequencer = mViewCtrl->getSequencer();

  if (controlBar) controlBar->setDisabled(false);
  if (sequencer) sequencer->setDisabled(false);

  if (!error) {
    sequencer->setPattern(pattern);
  }
}

void rg::App::onPatternChanged(PatternId pattern)
{
  SequencerRef sequencer = mViewCtrl->getSequencer();
  TensionCanvasRef canvas = mViewCtrl->getTensionCanvas();
  MTVRhythmSpace * space = mMtvRhythmSpace;

  if (sequencer) {
    sequencer->setPattern(pattern);
  }

  if (space) {
    if (space->ready() && canvas) {
      const Tension * const mtv = space->getMTV(pattern);
      canvas->setLockedTensionLine(mtv);
    }

    RhythmPattern rhythm(space->getTimeSignature(), space->getStepUnit(), pattern);
    mPatternPlayer.setPattern(rhythm);
  }
}

void rg::App::onTimeSignatureRequest(const TimeSignature& ts)
{
  MTVRhythmSpace * space = mMtvRhythmSpace;
  UnitRef unit = space ? space->getStepUnit() : Unit::QUAVER;
  resetMtvRhythmSpace(ts, unit);
}

void rg::App::onStepUnitRequest(UnitRef unit)
{
  MTVRhythmSpace * space = mMtvRhythmSpace;
  TimeSignature ts = space ? space->getTimeSignature() : TimeSignature(4, 4);
  resetMtvRhythmSpace(ts, unit);
}
