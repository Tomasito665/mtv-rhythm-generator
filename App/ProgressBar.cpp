#include "ProgressBar.h"
#include <poScene/ShapeView.h>


rg::ProgressBarRef rg::ProgressBar::create(double initialProgress, bool visible)
{
  ProgressBarRef ref(new ProgressBar);
  ref->setProgress(initialProgress);
  ref->setVisible(visible);
  ref->setup();
  return ref;
}

void rg::ProgressBar::setup()
{
  mFilledBackground = po::scene::ShapeView::createRect(1.0f, 1.0f);
  mStaticBackground = po::scene::ShapeView::createRect(1.0f, 1.0f);
  mFilledBackground->setSuperviewShouldIgnoreInBounds(true);
  mStaticBackground->setSuperviewShouldIgnoreInBounds(true);

  ci::TextBox textBox;
  textBox.setText("Loading...");
  textBox.setAlignment(ci::TextBox::Alignment::LEFT);
  textBox.setFont(ci::Font(textBox.getFont().getName(),
    getHeight() * RG_PROG_BAR_TEXT_RATIO));

  mTextView = po::scene::TextView::create(textBox);
  mTextView->setAlignment(po::scene::Alignment::BOTTOM_LEFT);
  addSubviews({ mStaticBackground, mFilledBackground, mTextView });
}

void rg::ProgressBar::update()
{
  const ci::vec2& pos = getBounds().getUpperLeft();
  const ci::vec2& size = getSize();

  mFilledBackground->setPosition(pos);
  mFilledBackground->setScale(mProgress * size.x, size.y);
  mFilledBackground->setFillColor(getStrokeColor());

  mStaticBackground->setPosition(pos);
  mStaticBackground->setScale(size);
  mStaticBackground->setFillColor(getFillColor());

  mTextView->setPosition(pos);
}

void rg::ProgressBar::draw()
{
  if (!mIsVisible) return;
  po::scene::View::draw();
  ci::gl::lineWidth(2);
  ci::gl::color(getStrokeColor());
  ci::gl::drawStrokedRect(getBounds());
}

void rg::ProgressBar::setTextColor(const ci::Color& color)
{
  ci::TextBox& textBox = mTextView->getCiTextBox();
  textBox.setColor(color);
  mTextView->render();
}

ci::Color rg::ProgressBar::getTextColor() const
{
  const ci::TextBox& textBox = mTextView->getCiTextBox();
  return textBox.getColor();
}

void rg::ProgressBar::setSize(ci::vec2 size)
{
  po::scene::View::setSize(size);
  ci::TextBox& textBox = mTextView->getCiTextBox();
  textBox.setFont(ci::Font(textBox.getFont().getName(), 
    size.y * RG_PROG_BAR_TEXT_RATIO));
}

rg::ProgressBar::ProgressBar()
{
}
