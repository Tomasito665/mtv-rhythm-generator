#pragma once

#include <cinder/audio/audio.h>
#include <thread>
#include <atomic>
#include "RhythmPattern.h"

#define DEFAULT_BPM 120
#define LAG_MS 10

namespace rg {
  class RhythmPatternPlayer
  {
  public:
    RhythmPatternPlayer();
    virtual ~RhythmPatternPlayer();

    void setup();  // must be called before startPolling() and startPlayback()
    void startPolling();  // start polling (immediately stops if setup() not called yet)
    void stopPolling(bool sync = false);  // stops polling (and joins to caller's thread if sync is true)

    void setPattern(const RhythmPattern& pattern); // sets the rhythm pattern
    RhythmPattern getPattern() const; // creates and returns a copy of the the rhythm pattern

    void startPlayback();
    void stopPlayback();
    inline bool isPlaying() const { return mIsPlaying; }
    void setLoopEnabled(bool enable = true);
    inline bool isLoopEnabled() const { return mLoop; }
    void setBpm(int bpm);
    inline int getBpm() const { return mBpm; }
    void setClick(bool enabled);
    inline bool getClick() const { return mClick; }
    void setBeatDuration(int steps);
    inline int getBeatDuration() const { return mNStepsPerBeat; }

    ci::signals::Signal<void(int step, bool isOnset)> sStep;
    ci::signals::Signal<void(bool playing)> sPlayback;

  protected:
    // Called from polling thread under lock, so it's safe to access the 
    // rhythm pattern from here. The given time is relative to the cinder 
    // audio context getNumProcessedSeconds().
    void handleStep(size_t step, double time);

  private:
    mutable std::mutex mMutex;
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
    std::atomic<bool> mClick;
    std::atomic<int> mNStepsPerBeat;

    // polling-thread access variables
    size_t mNextStepIx;
    double mNextStepTime;
    double mTimeStartPlayback;

    // main-thread access variables
    int mBpm;

    // cinder audio stuff
    ci::audio::Context * mAudioCtx;
    ci::audio::BufferPlayerNodeRef mSampleNode;
    ci::audio::BufferPlayerNodeRef mDownClickNode;
    ci::audio::BufferPlayerNodeRef mUpClickNode;
    ci::audio::GainNodeRef mSampleGainNode;
    ci::audio::GainNodeRef mClickGainNode;
  };
}
