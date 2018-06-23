#include "MTVCanvas.h"
#include <math.h>
#include <assert.h>


MTVCanvas::MTVCanvas() : MTVCanvas(16, Rectf(0, 0, 350, 350))
{
}

MTVCanvas::MTVCanvas(int dimensions, const Rectf & rect) :
  mRect(rect),
  mDimensions(dimensions),
  mFreeMtv(new Tension[dimensions]),
  mLockedMtv(new Tension[dimensions]),
  mShowProgress(false)
{
}

MTVCanvas::~MTVCanvas()
{
  if (mFreeMtv) {
    delete[] mFreeMtv;
    mFreeMtv = nullptr;
  }

  if (mLockedMtv) {
    delete[] mLockedMtv;
    mLockedMtv = nullptr;
  }
}

void MTVCanvas::reset()
{
  reset(mDimensions);
}

void MTVCanvas::reset(int dimensions)
{
  // deallocate old mtv data
  if (mFreeMtv) delete[] mFreeMtv;
  if (mLockedMtv) delete[] mLockedMtv;

  // allocate memory for new mtv data
  mFreeMtv = new Tension[dimensions]{};
  mLockedMtv = new Tension[dimensions]{};

  mDimensions = dimensions;
}

void MTVCanvas::updateLockedMtv(const Tension * const mtv)
{
  setMtv(mLockedMtv, mtv);

  for (int i = 0, n = mDimensions; i < n; ++i)
    mLockedMtv[i] = mtv[i];
}

void MTVCanvas::setRect(const Rectf & rect)
{
  mRect = rect;
  vec2 center = mRect.getCenter();
  mProgressBar.getRect().offsetCenterTo(center);
}

void MTVCanvas::update()
{
  float x = 0;
  float yFree, yLocked;
  float rHeight = mRect.getHeight();
  vec2 rUl = mRect.getUpperLeft();
  float rRightY = mRect.x1 + mRect.getWidth();

  // first-last values
  std::pair<float, float> freeMtvFiLaY = { mFreeMtv[0], mFreeMtv[mDimensions - 1] };
  std::pair<float, float> lockedMtvFiLaY = { mLockedMtv[0], mLockedMtv[mDimensions - 1] };

  // cyclic line slopes (at the borders)
  const float freeMtvCyclicOffsetY = (freeMtvFiLaY.second - freeMtvFiLaY.first) / 2.0f;
  const float lockedMtvCyclicOffsetY = (lockedMtvFiLaY.second - lockedMtvFiLaY.first) / 2.0f;

  // border y values
  const std::pair<float, float> freeMtvBoundY = { 
    freeMtvFiLaY.first + freeMtvCyclicOffsetY, 
    freeMtvFiLaY.second - freeMtvCyclicOffsetY
  };

  const std::pair<float, float> lockedMtvBoundY = {
    lockedMtvFiLaY.first + lockedMtvCyclicOffsetY, 
    lockedMtvFiLaY.second - lockedMtvCyclicOffsetY
  };

  mFreeMtvLinePath.clear();
  mLockedMtvLinePath.clear();

  mFreeMtvLinePath.moveTo(rUl.x, tension2YPos(freeMtvBoundY.first));
  mLockedMtvLinePath.moveTo(rUl.x, tension2YPos(lockedMtvBoundY.first));

  for (int i = 0, n = mDimensions; i < n; ++i) {
    x = (i + .5f) / n * mRect.getWidth() + mRect.x1;
    yFree = tension2YPos(mFreeMtv[i], rUl.y, rHeight);
    yLocked = tension2YPos(mLockedMtv[i], rUl.y, rHeight);

    mFreeMtvLinePath.lineTo(x, yFree);
    mLockedMtvLinePath.lineTo(x, yLocked);
  }

  mFreeMtvLinePath.lineTo(rRightY, tension2YPos(freeMtvBoundY.second));
  mLockedMtvLinePath.lineTo(rRightY, tension2YPos(lockedMtvBoundY.second));

  if (mShowProgress) mProgressBar.update();
}

void MTVCanvas::draw()
{
  gl::lineWidth(2);
  gl::color(Color(1, 1, 1));
  gl::drawStrokedRect(mRect);
  gl::color(Color(1, 0, 0));
  gl::draw(mFreeMtvLinePath);
  gl::color(Color(0, 1, 0));
  gl::draw(mLockedMtvLinePath);

  if (mShowProgress) {
    gl::color(Color(0, 1, 0));
    gl::drawSolidRect(mRect);
    mProgressBar.draw();
  }
}

void MTVCanvas::showProgressBar(bool show)
{
  mShowProgress = show;
}

void MTVCanvas::setProgressBarValue(double progress)
{
  mProgressBar.setProgress(progress);
}

void MTVCanvas::mouseDown(const MouseEvent& event)
{
  const vec2& pos = event.getPos();
  const bool mouseWasDown = mMouseDown;
  mMouseDown = mRect.contains(pos);

  if (!mMouseDown) {
    return;
  }

  if (event.isAltDown() && !mouseWasDown) {
    setMtv(mFreeMtv, mLockedMtv);
    return;
  }

  mouseOnCanvas(pos);
}

void MTVCanvas::mouseDrag(const MouseEvent& event)
{
  const vec2& pos = event.getPos();
  const bool wasDrawing = mMouseDown;

  if (mMouseDown = mRect.contains(pos))
    mouseOnCanvas(pos);
  else if (wasDrawing)
    sFreeMtvChanged.emit(mFreeMtv, mDimensions);
}

void MTVCanvas::mouseUp(const MouseEvent& event)
{
  const vec2& pos = event.getPos();
  if (mMouseDown && mRect.contains(pos))
    sFreeMtvChanged.emit(mFreeMtv, mDimensions);
  mMouseDown = false;
}

void MTVCanvas::mouseOnCanvas(vec2 pos)
{
  pos -= vec2(mRect.x1, mRect.y1);  // convert to local position
  const int dim = (int)std::round((float)pos.x / mRect.getWidth() * mDimensions);
  const float rHeight = mRect.getHeight();
  const Tension ten = (float)(rHeight - pos.y) / rHeight;
  mFreeMtv[dim] = ten;  // update free mtv
}

void MTVCanvas::setMtv(Tension * mtvSlave, const Tension * const mtvMaster)
{
  assert(mtvSlave == mLockedMtv || mtvSlave == mFreeMtv);
  for (size_t i = 0; i < mDimensions; ++i) {
    mtvSlave[i] = mtvMaster[i];
  }
}
