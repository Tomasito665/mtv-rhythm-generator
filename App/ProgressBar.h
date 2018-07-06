#pragma once

#include <cinder/Text.h>
#include <poScene/View.h>
#include <poScene/TextView.h>

#define RG_PROG_BAR_TEXT_RATIO 0.85

namespace rg {
  typedef std::shared_ptr<class ProgressBar> ProgressBarRef;

  class ProgressBar : public po::scene::View
  {
  public:
    static ProgressBarRef create(double initialProgress = 0.0, bool visible = false);

    void setup();
    void update() override;
    void draw() override;

    inline void setVisible(bool visible) { mIsVisible = visible; }
    inline bool isVisible() { return mIsVisible; }

    inline void setProgress(double progress) { mProgress = progress; }
    inline double getProgress() { return mProgress; }

    void setTextColor(const ci::Color& color);
    const ci::Color& getTextColor() const;

    void setSize(ci::vec2 size) override;

  private:
    ProgressBar();
    bool mIsVisible;
    double mProgress;
    ci::Rectf mMainRect;
    po::scene::ShapeViewRef mFilledBackground;
    po::scene::ShapeViewRef mStaticBackground;
    po::scene::TextViewRef mTextView;
  };
}
