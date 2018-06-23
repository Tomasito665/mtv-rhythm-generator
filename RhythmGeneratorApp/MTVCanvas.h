#pragma once

#include "ProgressBar.h"
#include "MTVRhythmSpace.h"
#include <cinder/app/App.h>
#include <cinder/gl/gl.h>
#include <vector>

using namespace ci;
using namespace ci::app;

class MTVCanvas
{
public:
  MTVCanvas();
  MTVCanvas(int dimensions, const Rectf& rect);
  virtual ~MTVCanvas();

  void reset();
  void reset(int dimensions);

  void updateLockedMtv(const Tension * const mtv);
  inline const Tension * const getLockedMtv() const { return mLockedMtv; }
  inline const Tension * const getFreeMtv() const { return mFreeMtv; }
  inline int getDimensions() const { return mDimensions; }
  
  void setRect(const Rectf& rect);
  void update();
  void draw();

  void showProgressBar(bool show = true);
  void setProgressBarValue(double progress);

  // supported events
  void mouseDown(const MouseEvent& event);
  void mouseDrag(const MouseEvent& event);
  void mouseUp(const MouseEvent& event);

  // signals
  signals::Signal<void(const Tension * const mtv, int dimensions)> sFreeMtvChanged;
  signals::Signal<void(const Tension * const mtv, int dimensions)> sLockedMtvChanged;

protected:
  void mouseOnCanvas(vec2 pos);
  inline float tension2YPos(Tension t) { return tension2YPos(t, mRect.y1, mRect.getHeight()); }
  static inline float tension2YPos(Tension t, float yUlRect, float heightRect) {
    return heightRect - ((float)t * heightRect) + yUlRect;
  }
  void setMtv(Tension * mtvSlave, const Tension * const mtvMaster);

private:
  Rectf mRect;
  size_t mDimensions;
  bool mMouseDown;
  Tension * mFreeMtv;
  Tension * mLockedMtv;
  Path2d mFreeMtvLinePath;
  Path2d mLockedMtvLinePath;
  ProgressBar mProgressBar;
  bool mShowProgress;
};
