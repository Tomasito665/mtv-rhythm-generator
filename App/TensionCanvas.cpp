#include "TensionCanvas.h"
#include <poScene/ShapeView.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "Theme.h"

using namespace std::placeholders;
using po::scene::MouseEvent;


rg::TensionCanvasRef rg::TensionCanvas::create(int numSteps)
{
  TensionCanvasRef ref = TensionCanvasRef(new TensionCanvas(numSteps));
  return ref;
}

void rg::TensionCanvas::viewDidLoad()
{
  ci::vec2 windowSize = ci::app::getWindowSize();
  po::scene::ViewRef canvasView = getView();
  const Theme& theme = Theme::get();

  // add background
  mBackground = po::scene::ShapeView::createRect(1, 1);
  mBackground->setFillColor(theme.grayDark);
  mBackground->setSuperviewShouldIgnoreInBounds(true);
  canvasView->addSubview(mBackground);

  // create tension line views
  mTensionLines[FREE_LINE] = TensionLine::create(
    mStepCount, 12.5f, 
    theme.grayBase,
    theme.primaryLight,
    theme.primaryBase
  );

  mTensionLines[LOCKED_LINE] = TensionLine::create(
    mStepCount, 12.5f, 
    theme.grayBase,
    theme.grayDark,
    theme.grayBase
  );
  
  for (const auto& lineView : mTensionLines)
    canvasView->addSubview(lineView);

  // setup mouse events for the drawable line
  mTensionLines[FREE_LINE]->getSignal(MouseEvent::DRAG).connect(std::bind(&TensionCanvas::onMouseOnCanvas, this, _1));
  mTensionLines[FREE_LINE]->getSignal(MouseEvent::DOWN_INSIDE).connect(std::bind(&TensionCanvas::onMouseDownInside, this, _1));
  mTensionLines[FREE_LINE]->getSignal(MouseEvent::UP).connect(std::bind(&TensionCanvas::onMouseUp, this, _1));

  // add loading bar
  mLoadingBar = ProgressBar::create(0.0, true);
  mLoadingBar->setTextColor(theme.white);
  mLoadingBar->setStrokeColor(theme.primaryLight);
  mLoadingBar->setFillColor(theme.primaryDark);
  mLoadingBar->setSize(ci::vec2(200, 25));
  mLoadingBar->setAlignment(po::scene::Alignment::CENTER_CENTER);
  canvasView->addSubview(mLoadingBar);

  // add mtv distance label
  ci::TextBox mtvdTextBox;
  mtvdTextBox.setColor(theme.white);
  mtvdTextBox.setAlignment(ci::TextBox::Alignment::RIGHT);
  mtvdTextBox.setFont(ci::Font(mtvdTextBox.getFont().getName(), 32.0));
  mtvdTextBox.setText("Distance: 0.00");
  mMtvDistanceTextView = po::scene::TextView::create(mtvdTextBox);
  mMtvDistanceTextView->setAlignment(po::scene::Alignment::TOP_RIGHT);
  canvasView->addSubview(mMtvDistanceTextView);
}

void rg::TensionCanvas::update()
{
  po::scene::ViewRef canvas = getView();
  const ci::Rectf& bounds = canvas->getBounds();
  const ci::vec2& size = canvas->getSize();
  const ci::vec2& pos = bounds.getUpperLeft();
  const ci::vec2 lineViewSize = ci::vec2(size.x, size.y - mYPadding * 2.0f);
  const ci::vec2 lineViewPos = ci::vec2(pos.x, pos.y + mYPadding);

  const double loadingProgress = mLoadingProgress;
  const bool loading = mIsLoading;

  mBackground->setPosition(bounds.getUpperLeft());
  mBackground->setScale(size);

  mLoadingBar->setVisible(loading);
  if (loading) {
    mLoadingBar->setProgress(loadingProgress);
    mLoadingBar->setPosition(bounds.getCenter());
  }

  for (auto& lineView : mTensionLines) {
    lineView->setPosition(lineViewPos);
    lineView->setSize(lineViewSize);
    lineView->setEnabled(!loading);
  }

  if (mMtvDistance != mMtvDistancePrevUpdate) {
    std::string s = "Distance: N/A";

    if (mMtvDistance >= 0.0f) {
      std::stringstream sstream;
      sstream << "Distance: " << std::fixed << std::setprecision(3) << mMtvDistance;
      s = sstream.str();
    }

    ci::TextBox& mtvdTextBox = mMtvDistanceTextView->getCiTextBox();
    mtvdTextBox.setText(s);
    mMtvDistanceTextView->setSize(mtvdTextBox.measure());
    mMtvDistanceTextView->render();
  }

  mMtvDistanceTextView->setPosition(bounds.getUpperRight());
}

void rg::TensionCanvas::setStepCount(int stepCount)
{
  mStepCount = stepCount;
  mTensionLines[FREE_LINE]->setNodeCount(mStepCount, true);
  mTensionLines[LOCKED_LINE]->setNodeCount(mStepCount, false);
  notifyTensionLineChange();
}

void rg::TensionCanvas::setMtvDistance(float distance)
{
  mMtvDistance = distance;
}

void rg::TensionCanvas::setFreeTensionLine(const Tension * const data)
{
  mTensionLines[FREE_LINE]->setData(data);
  notifyTensionLineChange(FREE_LINE);
}

void rg::TensionCanvas::setLockedTensionLine(const Tension * const data)
{
  mTensionLines[LOCKED_LINE]->setData(data);
  notifyTensionLineChange(LOCKED_LINE);
}

const Tension * const rg::TensionCanvas::getFreeTensionLine() const
{
  return mTensionLines[FREE_LINE]->getData();
}

const Tension * const rg::TensionCanvas::getLockedTensionLine() const
{
  return mTensionLines[LOCKED_LINE]->getData();
}

void rg::TensionCanvas::resetTensionLines()
{
  for (const auto& lineView : mTensionLines)
    lineView->resetData();
  notifyTensionLineChange();
}

void rg::TensionCanvas::setYPadding(float yPadding)
{
  mYPadding = yPadding;
}

void rg::TensionCanvas::setLoadingProgress(double progress)
{
  return mLoadingProgress.store(progress);
}

double rg::TensionCanvas::getLoadingProgress()
{
  return mLoadingProgress.load();
}

rg::TensionCanvas::TensionCanvas(int numSteps, float yPadding) :
  mStepCount(numSteps),
  mYPadding(yPadding)
{
}

void rg::TensionCanvas::onMouseOnCanvas(MouseEvent& event)
{
  TensionLineRef freeLineView = mTensionLines[FREE_LINE];
  const ci::vec2& pos = event.getLocalPos(); // FREE_LINE tension view local pos
  const Tension tension = TensionLine::yPos2Tension(pos.y, freeLineView->getHeight());
  int step = TensionLine::xPos2Step(pos.x, mStepCount, freeLineView->getWidth());
  step = std::max(0, std::min(step, mStepCount - 1));  // <- clamp to [0, N - 1]

  if (mMouseDown && tension != freeLineView->getStep(step)) {
    freeLineView->setStep(step, std::max(0.0f, std::min(tension, 1.0f)));
    freeLineView->blimpNode(step);
    notifyTensionLineChange(FREE_LINE);
  }
}

void rg::TensionCanvas::onMouseDownInside(po::scene::MouseEvent& event)
{
  mMouseDown = true;
  onMouseOnCanvas(event);
}

void rg::TensionCanvas::onMouseUp(po::scene::MouseEvent& event)
{
  mMouseDown = false;
}

void rg::TensionCanvas::notifyTensionLineChange(int line)
{
  const int N = mStepCount;
  const Tension * const freeMtv = mTensionLines[FREE_LINE]->getData();
  const Tension * const lockedMtv = mTensionLines[LOCKED_LINE]->getData();

  if (line == FREE_LINE || line < 0)
    sFreeTensionLineChanged.emit(freeMtv, N);
  if (line == LOCKED_LINE || line < 0)
    sFreeTensionLineChanged.emit(lockedMtv, N);

  sTensionLineChanged.emit(freeMtv, lockedMtv, N);
}
