#include "RhythmPatternPlayer.h"
#include <cinder/app/App.h>
#include <thread>
#include "Resources.h"

rg::RhythmPatternPlayer::RhythmPatternPlayer()
{
  mAudioCtx = ci::audio::Context::master();
  mBpm = DEFAULT_BPM;
  // initializes the pattern duration to something that makes 
  // sense (even though the default pattern is empty)
  setPattern(mPattern);
}

rg::RhythmPatternPlayer::~RhythmPatternPlayer()
{
  if (mIsPollingThreadRunning)
    stopPolling(true);
}

void rg::RhythmPatternPlayer::setup()
{
  ci::DataSourceRef kickDrumSnd = ci::app::loadResource(RES_KICK_WAV);
  ci::DataSourceRef downClickSnd = ci::app::loadResource(RES_DOWN_CLICK_WAV);
  ci::DataSourceRef upClickSnd = ci::app::loadResource(RES_UP_CLICK_WAV);

  ci::audio::SourceFileRef kickSrc = ci::audio::load(kickDrumSnd, mAudioCtx->getSampleRate());
  ci::audio::SourceFileRef downClickSrc = ci::audio::load(downClickSnd);
  ci::audio::SourceFileRef upClickSrc = ci::audio::load(upClickSnd);

  mSampleNode = mAudioCtx->makeNode(new ci::audio::BufferPlayerNode(kickSrc->loadBuffer()));
  mDownClickNode = mAudioCtx->makeNode(new ci::audio::BufferPlayerNode(downClickSrc->loadBuffer()));
  mUpClickNode = mAudioCtx->makeNode(new ci::audio::BufferPlayerNode(upClickSrc->loadBuffer()));

  mSampleGainNode = mAudioCtx->makeNode(new ci::audio::GainNode(0.75f));
  mClickGainNode = mAudioCtx->makeNode(new ci::audio::GainNode(0.50f));

  mSampleNode >> mSampleGainNode >> mAudioCtx->getOutput();
  mDownClickNode >> mClickGainNode >> mAudioCtx->getOutput();
  mUpClickNode >> mClickGainNode >> mAudioCtx->getOutput();

  mAudioCtx->enable();
}

void rg::RhythmPatternPlayer::startPolling()
{
  bool exp = false;
  if (mIsPollingThreadRunning.compare_exchange_strong(exp, true)) {
    mPollingThread = std::thread(std::bind(&RhythmPatternPlayer::pollingThreadFunc, this));
  }
}

void rg::RhythmPatternPlayer::stopPolling(bool sync)
{
  {
    std::lock_guard<std::mutex> lock(mMutex);
    mIsPollingThreadRunning.store(false);
    if (!mIsPlaying) mPollingCV.notify_all();
  }

  if (sync) {
    mPollingThread.join();
  }
}

void rg::RhythmPatternPlayer::setPattern(const RhythmPattern& pattern)
{
  const TimeSignature& timesig = mPattern.getTimeSignature();
  const double measureInCrotchets = timesig.getMeasureDuration(Unit::CROTCHET);
  std::lock_guard<std::mutex> lock(mMutex);
  mPatternDuration = 60.0 / mBpm * measureInCrotchets;
  mPattern = pattern;
}

RhythmPattern rg::RhythmPatternPlayer::getPattern() const
{
  std::lock_guard<std::mutex> lock(mMutex);
  return mPattern;
}

void rg::RhythmPatternPlayer::startPlayback()
{
  mIsPlaying = true;
  mPollingCV.notify_all();
  sPlayback.emit(true);
}

void rg::RhythmPatternPlayer::stopPlayback()
{
  mIsPlaying = false;
  sPlayback.emit(false);
}

void rg::RhythmPatternPlayer::setLoopEnabled(bool enable)
{
  mLoop = enable;
}

void rg::RhythmPatternPlayer::setBpm(int bpm)
{
  mBpm = std::max(32, std::min(300, bpm));
}

void rg::RhythmPatternPlayer::setClick(bool enabled)
{
  mClick = enabled;
}

void rg::RhythmPatternPlayer::setBeatDuration(int steps)
{
  mNStepsPerBeat = steps;
}

void rg::RhythmPatternPlayer::handleStep(size_t step, double time)
{
  const bool isOnset = mPattern[step];
  time += LAG_MS / 1000.0;

  if (mClick && !(step % mNStepsPerBeat)) {
    (step == 0 ? mDownClickNode : mUpClickNode)->start(time);
  }

  if (isOnset) {
    mSampleNode->start(time);
  }

  sStep.emit(step, isOnset);
}

void rg::RhythmPatternPlayer::pollingThreadFunc()
{
  if (!mAudioCtx->isEnabled()) {
    mIsPollingThreadRunning = false;
    return;
  }

  while (mIsPollingThreadRunning) {
    if (!mIsPlaying) {
      // When playback is disabled, block the polling thread until 
      // playback is enabled with startPlayback().
      std::unique_lock<std::mutex> lock(mMutex);
      mPollingCV.wait(lock);

      // If stopPolling() was called while waiting for the 
      // condition variable (while not playing).
      if (!mIsPollingThreadRunning) {
        break;
      }

      // Seek to start of the rhythm pattern.
      mTimeStartPlayback = mAudioCtx->getNumProcessedSeconds();
      mNextStepTime = 0;
      mNextStepIx = 0;
    }

    poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void rg::RhythmPatternPlayer::poll()
{
  const double time = mAudioCtx->getNumProcessedSeconds();
  const double absStepTime = mTimeStartPlayback + mNextStepTime;

  if (time >= absStepTime) {
    std::lock_guard<std::mutex> lock(mMutex);
    const size_t numSteps = (size_t)mPattern.getNSteps();
    handleStep(mNextStepIx, absStepTime);

    if (++mNextStepIx >= numSteps) {
      if (!mLoop) { mIsPlaying = false; return; }
      mTimeStartPlayback = mTimeStartPlayback + mPatternDuration;
      mNextStepIx = 0;
    }

    mNextStepTime = mPatternDuration * (double)mNextStepIx / numSteps;
  }
}
