#pragma once

#include <cinder/app/App.h>
#include <cinder/app/RendererGl.h>
#include <cinder/params/Params.h>
#include <poScene/Scene.h>
#include "MainViewController.h"
#include "KeyboardController.h"
#include "RhythmPatternPlayer.h"

class MTVRhythmSpace;

namespace rg {
  class App : public ci::app::App 
  {
  public:
    static void prepareSettings(Settings * settings);

    void setup() override;
    void update() override;
    void draw() override;
    void cleanup() override;

    void keyUp(ci::app::KeyEvent event) override;
    void keyDown(ci::app::KeyEvent event) override;

    // tries to create a new mtv rhythm space and fills it asynchronously, returns 
    // false if it failed to do so (because current space is busy)
    void resetMtvRhythmSpace(const TimeSignature& ts, UnitRef unit);

    void setPatternClosestToMtv();
    void setRandomPatternCloseToMtv();

    void toggleRhythmPatternPlayerPlayback();
    void toggleRhythmPatternPlayerLoop();
    void toggleRhythmPatternPlayerClick();

  protected:
    void checkFutures();  // called at every update()
    void setupTheme();
    void setupSignals();

    // async action callbacks
    void onMtvRhythmSpaceReset(bool error = false);
    void onGetRandomPatternCloseToMtv(PatternId pattern, bool error = false);

    void onTensionLineChanged(const Tension * const freeMtv, const Tension * const lockedMtv, int nSteps);
    void onPatternChanged(PatternId pattern);
    void onTimeSignatureRequest(const TimeSignature& ts);
    void onStepUnitRequest(UnitRef unit);

  private:
    std::mutex mMutex;
    std::shared_future<void> mFutureRhythmSpaceReset;
    std::shared_future<PatternId> mFutureGetRandomPatternCloseToMtv;

    po::scene::SceneRef mScene;
    MainViewControllerRef mViewCtrl;
    KeyboardController mKbdController;
    RhythmPatternPlayer mPatternPlayer;
    MTVRhythmSpace * mMtvRhythmSpace;
  };
}

CINDER_APP(rg::App, ci::app::RendererGl, &rg::App::prepareSettings);
