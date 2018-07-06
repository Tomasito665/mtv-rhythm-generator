#pragma once

#include <poScene/ViewController.h>
#include "Theme.h"
#include "Sequencer.h"
#include "ControlBar.h"
#include "TensionCanvas.h"

namespace rg {
  typedef std::shared_ptr<class MainViewController> MainViewControllerRef;

  class MainViewController : public po::scene::ViewController
  {
  public:
    static MainViewControllerRef create();
    void viewDidLoad() override;

    inline SequencerRef getSequencer() { return mSequencer; }
    inline TensionCanvasRef getTensionCanvas() { return mTensionCanvas; }
    inline ControlBarRef getControlBar() { return mControlBar; }

  private:
    MainViewController();
    ControlBarRef mControlBar;
    SequencerRef mSequencer;
    TensionCanvasRef mTensionCanvas;
  };
}
