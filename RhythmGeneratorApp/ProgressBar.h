#pragma once

#include <cinder/app/App.h>
#include <atomic>

using namespace ci;
using namespace ci::app;

class ProgressBar
{
public:
  ProgressBar();
  ProgressBar(double initProgress, const Rectf& rect);
  ~ProgressBar();

  void setRect(const Rectf& rect);
  Rectf& getRect();

  void update();
  void draw();
  
  void setProgress(double progress);

private:
  Rectf mMainRect;
  Rectf mFilledRect;
  std::atomic<double> mProgress;
};
