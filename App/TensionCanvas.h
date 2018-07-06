#pragma once

#include <poScene/ViewController.h>
#include <poScene/TextView.h>
#include <cinder/Signals.h>
#include "TensionLine.h"
#include "ProgressBar.h"

namespace rg {
  typedef std::shared_ptr<class TensionCanvas> TensionCanvasRef;

  class TensionCanvas : public po::scene::ViewController
  {
  public:
    static TensionCanvasRef create(int numSteps = 16);

    void viewDidLoad() override;
    void update() override;

    void setStepCount(int stepCount);
    inline int getStepCount() const { return mStepCount; }

    void setMtvDistance(float distance);
    void setFreeTensionLine(const Tension * const data);
    void setLockedTensionLine(const Tension * const data);
    const Tension * const getFreeTensionLine() const;
    const Tension * const getLockedTensionLine() const;
    void resetTensionLines();

    void setYPadding(float yPadding);
    inline float getYPadding() const { return mYPadding; }

    inline void setLoading(bool loading) { mIsLoading = loading; }
    inline bool isLoading() const { return mIsLoading; };
    void setLoadingProgress(double progress);
    double getLoadingProgress();

    ci::signals::Signal<void(const Tension * const mtv, int stepCount)> sLockedTensionLineChanged;
    ci::signals::Signal<void(const Tension * const mtv, int stepCount)> sFreeTensionLineChanged;
    ci::signals::Signal<void(const Tension * const freeMtv, const Tension * const lockedMtv, int stepCount)> sTensionLineChanged;

  protected:
    explicit TensionCanvas(int numSteps, float yPadding = 25.0f);
    void onMouseOnCanvas(po::scene::MouseEvent& event);
    void onMouseDownInside(po::scene::MouseEvent& event);
    void onMouseUp(po::scene::MouseEvent& event);
    void notifyTensionLineChange(int line = -1); // < 0 for both lines

  private:
    int mStepCount;
    float mYPadding;
    bool mMouseDown;
    enum { FREE_LINE = 0, LOCKED_LINE = 1 };
    po::scene::ShapeViewRef mBackground;
    TensionLineRef mTensionLines[2]; // contains free and locked lines
    bool mIsLoading;
    ProgressBarRef mLoadingBar;
    float mMtvDistance, mMtvDistancePrevUpdate;
    po::scene::TextViewRef mMtvDistanceTextView;
    std::atomic<double> mLoadingProgress;
  };
}
