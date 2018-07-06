#pragma once

#include <cinder/Signals.h>
#include <poScene/View.h>
#include "MultiSwitchOption.h"

using namespace std::placeholders;


namespace rg {
  template <typename T>
  class MultiSwitch : public po::scene::View
  {
  public:
    MultiSwitch(const std::string& title, float height, float relativeYPadding = 0.1);

    void setup();
    void update() override;

    void addOption(const std::string& label, T value);
    void setDisabled(bool disabled);

    T get() const;
    void set(T value);
    void setByIndex(int ix);

    ci::signals::Signal<void(int optionIx, T value)> sOptionClicked;
   
  protected:
    void updateOptionViewPositions();
    void onClickOption(int optionIx);

  private:
    struct Option
    {
      MultiSwitchOptionRef view;
      std::string label;
      T value;
    };

    int mSelectedOptionIx;
    bool mShouldUpdateOptionViewPositions;
    std::vector<Option> mOptions;
    std::unordered_map<T, int> mOptionIndicesByValue;

    float mTitleMargin;
    ci::Font mOptionFont;
    ci::TextBox mTitleTextBox;
    po::scene::TextViewRef mTitleTextView;

    typedef std::pair<ci::Color, ci::Color> ColorPair;
    ColorPair mOptBaseColors;
    ColorPair mOptActiveColors;
    ColorPair mOptTextColors;
  };
}

template<typename T>
inline rg::MultiSwitch<T>::MultiSwitch(const std::string& title, float height, float relativeYPadding) : 
  mSelectedOptionIx(-1),
  mShouldUpdateOptionViewPositions(true)
{
  relativeYPadding = std::max(0.0f, std::min(1.0f, relativeYPadding));  // clamp to [0, 1]
  const float optionFontSize = height * (1.0f - relativeYPadding * 2.0f);
  const float titleFontSize = optionFontSize * 0.75f;

  const std::string& fontName = mTitleTextBox.getFont().getName();
  const ci::Font titleFont(fontName, titleFontSize);
  mOptionFont = ci::Font(fontName, optionFontSize);
  mTitleMargin = titleFontSize * 0.15f;

  mTitleTextBox.setText(title);
  mTitleTextBox.setFont(titleFont);
  mTitleTextBox.setAlignment(ci::TextBox::Alignment::RIGHT);

  const Theme& theme = Theme::get();
  mOptBaseColors = { theme.black, theme.grayDark };
  mOptActiveColors = { theme.primaryBase, theme.primaryLight };
  mOptTextColors = { theme.grayBase, ci::Color(1, 1, 1) };
}

template<typename T>
inline void rg::MultiSwitch<T>::setup()
{
  const ci::vec2 titleSize = mTitleTextBox.measure();
  mTitleTextView = po::scene::TextView::create(mTitleTextBox);
  mTitleTextView->setSize(titleSize);
  mTitleTextView->setPosition(mTitleMargin, 0.0f);
  addSubview(mTitleTextView);
}

template<typename T>
inline void rg::MultiSwitch<T>::update()
{
  if (mShouldUpdateOptionViewPositions) {
    updateOptionViewPositions();
    mShouldUpdateOptionViewPositions = false;
  }

  const int nOptions = mOptions.size();
  for (int i = 0; i < nOptions; ++i) {
    MultiSwitchOptionRef view = mOptions[i].view;
    view->setActive(i == mSelectedOptionIx);
  }
}

template<typename T>
inline void rg::MultiSwitch<T>::addOption(const std::string& label, T value)
{
  const int optionIndex = mOptions.size();

  // assert that there's no option with the given value
  assert(mOptionIndicesByValue.find(value) == mOptionIndicesByValue.end());

  // create and setup option view
  MultiSwitchOptionRef optView = MultiSwitchOption::create(label, optionIndex, mOptionFont);
  optView->setColorBase(mOptBaseColors.first);
  optView->setColorBaseHover(mOptBaseColors.second);
  optView->setColorActive(mOptActiveColors.first);
  optView->setColorActiveHover(mOptActiveColors.second);
  optView->setAlignment(po::scene::Alignment::CENTER_LEFT);
  optView->setTextColorBase(mOptTextColors.first);
  optView->setTextColorActive(mOptTextColors.second);
  optView->sClicked.connect(std::bind(&MultiSwitch::onClickOption, this, _1));

  mOptions.push_back({ optView, label, value });
  mOptionIndicesByValue[value] = optionIndex;
  addSubview(optView);

  if (mSelectedOptionIx < 0) mSelectedOptionIx = optionIndex;
}

template<typename T>
inline void rg::MultiSwitch<T>::setDisabled(bool disabled)
{
  for (const Option& opt : mOptions) {
    opt.view->setDisabled(disabled);
  }
}

template<typename T>
inline T rg::MultiSwitch<T>::get() const
{
  return mOptions[mSelectedOptionIx].value;
}

template<typename T>
inline void rg::MultiSwitch<T>::set(T value)
{
  // assert that there's an option with the given value
  assert(mOptionIndicesByValue.find(value) != mOptionIndicesByValue.end());
  mSelectedOptionIx = mOptionIndicesByValue[value];
}

template<typename T>
inline void rg::MultiSwitch<T>::setByIndex(int ix)
{
  assert(ix >= 0 && ix < mOptions.size());
  mSelectedOptionIx = ix;
}

template<typename T>
inline void rg::MultiSwitch<T>::updateOptionViewPositions()
{
  float y = mTitleTextView->getBounds().getCenter().y;
  ci::vec2 prevObjPos = mTitleTextView->getPosition() + ci::vec2(mTitleMargin, 0.0f);
  ci::vec2 prevObjSize = mTitleTextView->getSize();
  ci::Rectf container();

  for (Option& opt : mOptions) {
    po::scene::ViewRef view = opt.view;
    view->setPosition(prevObjPos + ci::vec2(prevObjSize.x, y));
    prevObjPos = view->getPosition();
    prevObjSize = view->getSize();
    y = 0;
  }
}

template<typename T>
inline void rg::MultiSwitch<T>::onClickOption(int optionIx)
{
  const Option& opt = mOptions[optionIx];
  sOptionClicked.emit(optionIx, opt.value);
}
