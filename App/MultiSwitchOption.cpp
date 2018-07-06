#include "MultiSwitchOption.h"
#include <cinder/Font.h>
#include <poScene/ShapeView.h>

rg::MultiSwitchOptionRef rg::MultiSwitchOption::create(const std::string& label, int optionIx, const ci::Font& font)
{
  MultiSwitchOptionRef ref(new MultiSwitchOption(label, optionIx, font));
  ref->setup();
  return ref;
}

void rg::MultiSwitchOption::setup()
{
  ToggleButton::setup();
  mTextView = po::scene::TextView::create(mTextBox);
  addSubview(mTextView);
  updateTextColor();
}

void rg::MultiSwitchOption::update()
{
  ToggleButton::update();
  bool isActiveOnCurrUpdate = isActive();
  if (isActiveOnCurrUpdate != mWasActiveOnPrevUpdate) updateTextColor();
  mWasActiveOnPrevUpdate = isActiveOnCurrUpdate;
}

po::scene::ShapeViewRef rg::MultiSwitchOption::createToggleShape()
{
  const ci::vec2& size = mTextBox.measure();
  return po::scene::ShapeView::createRect(size.x, size.y);
}

void rg::MultiSwitchOption::onClick()
{
  sClicked.emit(mOptionIx);
}

void rg::MultiSwitchOption::updateTextColor()
{
  mTextBox.setColor(isActive() ? mTextColorActive : mTextColorBase);
  mTextView->setCiTextBox(mTextBox);
}

rg::MultiSwitchOption::MultiSwitchOption(const std::string& label, int optionIx, const ci::Font& font) :
  mOptionIx(optionIx)
{
  mTextBox.setSize(ci::vec2(ci::TextBox::GROW, ci::TextBox::GROW));
  mTextBox.alignment(ci::TextBox::Alignment::CENTER);
  mTextBox.setFont(font);
  mTextBox.setText(label);
}
