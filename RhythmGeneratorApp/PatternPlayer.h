#pragma once

#include "RhythmPattern.h"
#include <cinder/audio/audio.h>
#include <thread>
#include <atomic>

#define DEFAULT_BPM 120
#define LAG_MS 10

using namespace ci;

class PatternPlayer
{
public:
  PatternPlayer();
  virtual ~PatternPlayer();

  void setup();  // must be called before startPolling() and startPlayback()
  void startPolling();  // start polling (immediately stops if setup() not called yet)
  void stopPolling(bool sync = false);  // stops polling (and joins to caller's thread if sync is true)

  void setPattern(const RhythmPattern& pattern); // sets the rhythm pattern
  RhythmPattern getPattern(); // creates and returns a copy of the the rhythm pattern

  void startPlayback();
  void stopPlayback();
  inline bool isPlaying() const { return mIsPlaying; }
  void enableLoop(bool enable = true);
  inline bool isLoopEnabled() const { return mLoop; }
  void setBpm(int bpm);
  inline int getBpm() const { return mBpm; }

protected:
  // Called from polling thread under lock, so it's safe to access the 
  // rhythm pattern from here. The given time is relative to the cinder 
  // audio context getNumProcessedSeconds().
  void handleStep(size_t step, double time);

private:
  std::mutex mMutex;
  std::thread mPollingThread;
  std::condition_variable mPollingCV;  // blocks polling thread while playback is inactive
  
  void pollingThreadFunc();  // the polling thread executes this function
  void poll();               // called from polling thread

  // shared-access variables under lock
  RhythmPattern mPattern;
  double mPatternDuration;

  // shared-access variables sometimes under lock
  std::atomic<bool> mIsPollingThreadRunning;
  std::atomic<bool> mIsPlaying;
  std::atomic<bool> mLoop;

  // polling-thread access variables
  size_t mNextStepIx;
  double mNextStepTime;
  double mTimeStartPlayback;

  // main-thread access variables
  int mBpm;

  // cinder audio stuff
  audio::Context * mAudioCtx;
  audio::BufferPlayerNodeRef mBufferPlayerNode;
  audio::GainNodeRef mGainNode;
};
