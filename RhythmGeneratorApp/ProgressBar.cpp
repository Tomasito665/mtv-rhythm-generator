#include "..\App\ProgressBar.h"
#include "..\App\ProgressBar.h"
#include "..\App\ProgressBar.h"
#include "ProgressBar.h"
#include <algorithm>
#include <cinder/gl/gl.h>

ProgressBar::ProgressBar() : ProgressBar(0.0, Rectf(0, 0, 250, 10))
{
}

ProgressBar::ProgressBar(double initProgress, const Rectf& rect) : 
  mMainRect(rect),
  mProgress(initProgress)
{
}


ProgressBar::~ProgressBar()
{
}

void ProgressBar::setRect(const Rectf & rect)
{
  mMainRect = rect;
}

Rectf& ProgressBar::getRect()
{
  return mMainRect;
}

void ProgressBar::update()
{
  const vec2 mainUL = mMainRect.getUpperLeft();
  const float mainWidth = mMainRect.getWidth();
  const float mainHeight = mMainRect.getHeight();
  const float filledWidth = mainWidth * (float)mProgress.load();
  mFilledRect = Rectf(mainUL, mainUL + vec2(filledWidth, mainHeight));
}

void ProgressBar::draw()
{
  gl::lineWidth(1);
  gl::color(Color(1, 1, 1));
  gl::drawStrokedRect(mMainRect);
  gl::drawSolidRect(mFilledRect);
}

void ProgressBar::setProgress(double progress)
{
  // clamp progress to [0, 1]
  mProgress.store(std::max(0.0, std::min(progress, 1.0)));
}

double rg::ProgressBar::getProgress()
{
  return mProgress.load();
}
