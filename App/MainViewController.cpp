#include "MainViewController.h"

rg::MainViewControllerRef rg::MainViewController::create()
{
  rg::MainViewControllerRef ref(new MainViewController);
  return ref;
}

void rg::MainViewController::viewDidLoad()
{
  ci::vec2 winSize = ci::app::getWindowSize();
  ci::vec2 winCenter = ci::app::getWindowCenter();

  mControlBar = ControlBar::create();
  mTensionCanvas = TensionCanvas::create(16);
  mSequencer = Sequencer::create(16);

  po::scene::ViewRef mainView = getView();
  po::scene::ViewRef tenCanvasView = mTensionCanvas->getView();
  po::scene::ViewRef sequencerView = mSequencer->getView();
  po::scene::ViewRef ctrlBarView = mControlBar->getView();
  
  ctrlBarView->setSize(ci::vec2(winSize.x, winSize.y * 0.10f));
  tenCanvasView->setSize(ci::vec2(winSize.x, winSize.y * 0.765f));
  sequencerView->setSize(ci::vec2(winSize.x, winSize.y * 0.135f));

  ctrlBarView->setPosition(0, 0);
  tenCanvasView->setPosition(0, ctrlBarView->getHeight());
  sequencerView->setPosition(0, ctrlBarView->getHeight() + tenCanvasView->getHeight());

  mainView->addSubviews({ ctrlBarView, tenCanvasView, sequencerView });
  mainView->setAlignment(po::scene::Alignment::CENTER_CENTER);
  mainView->setSize(winSize);
  mainView->setPosition(winCenter);
}

rg::MainViewController::MainViewController()
{
}
