#include "PatternPlayer.h"
#include "Resources.h"
#include <cinder/app/App.h>
#include <cinder/audio/audio.h>
#include <cinder/Log.h>

using namespace ci;
using namespace ci::app;
using namespace ci::log;

PatternPlayer::PatternPlayer() {
  mAudioCtx = audio::Context::master();
  mBpm = DEFAULT_BPM;
  // initializes the pattern duration to something that makes 
  // sense (even though the default pattern is empty)
  setPattern(mPattern);
}

PatternPlayer::~PatternPlayer() {
  if (mIsPollingThreadRunning)
    stopPolling(true);
}

void PatternPlayer::setup() {
  audio::SourceFileRef sourceFile = audio::load(loadResource(RES_KICK_WAV), mAudioCtx->getSampleRate());
  audio::BufferRef buffer = sourceFile->loadBuffer();
  mBufferPlayerNode = mAudioCtx->makeNode(new audio::BufferPlayerNode(buffer));
  mGainNode = mAudioCtx->makeNode(new audio::GainNode(0.5f));
  mBufferPlayerNode >> mGainNode >> mAudioCtx->getOutput();
  mAudioCtx->enable();
}

void PatternPlayer::startPolling()
{
  bool exp = false;
  if (mIsPollingThreadRunning.compare_exchange_strong(exp, true)) {
    mPollingThread = std::thread(std::bind(&PatternPlayer::pollingThreadFunc, this));
  }
}

void PatternPlayer::stopPolling(bool sync)
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

void PatternPlayer::setPattern(const RhythmPattern& pattern)
{
  const TimeSignature& timesig = mPattern.getTimeSignature();
  const double measureInCrotchets = timesig.getMeasureDuration(Unit::CROTCHET);
  std::lock_guard<std::mutex> lock(mMutex);
  mPatternDuration = 60.0 / mBpm * measureInCrotchets;
  mPattern = pattern;
}

RhythmPattern PatternPlayer::getPattern()
{
  std::lock_guard<std::mutex> lock(mMutex);
  return mPattern;
}

void PatternPlayer::startPlayback()
{
  mIsPlaying = true;
  mPollingCV.notify_all(); 
}

void PatternPlayer::stopPlayback()
{
  mIsPlaying = false;
}

void PatternPlayer::enableLoop(bool enable)
{
  mLoop = enable;
}

void PatternPlayer::setBpm(int bpm)
{
  mBpm = std::max(32, std::min(300, bpm));
}

void PatternPlayer::handleStep(size_t step, double time)
{
  if (mPattern[step]) {
    mBufferPlayerNode->start(time + LAG_MS / 1000.0);
  }
}

void PatternPlayer::pollingThreadFunc()
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

void PatternPlayer::poll()
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
