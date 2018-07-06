#pragma once

#include <cinder/Text.h>
#include <poScene/View.h>
#include <poScene/TextView.h>
#include "ToggleButton.h"

namespace rg {
  typedef std::shared_ptr<class MultiSwitchOption> MultiSwitchOptionRef;

  class MultiSwitchOption : public ToggleButton
  {
  public:
    static MultiSwitchOptionRef create(const std::string& label, int optionIx, const ci::Font& font);
    ci::signals::Signal<void(int optionIx)> sClicked;

    void setup() override;
    void update() override;

    inline void setTextColorBase(const ci::Color& color) { mTextColorBase = color; updateTextColor(); }
    inline void setTextColorActive(const ci::Color& color) { mTextColorActive = color; updateTextColor(); }
    inline const ci::Color& getTextColorBase() const { return mTextColorBase; }
    inline const ci::Color& getTextColorActive() const { return mTextColorActive; }

  protected:
    po::scene::ShapeViewRef createToggleShape() override;
    void onClick() override;
    void updateTextColor();

  private:
    MultiSwitchOption(const std::string& label, int optionIx, const ci::Font& font);
    int mOptionIx;
    bool mWasActiveOnPrevUpdate;
    ci::Color mTextColorBase;
    ci::Color mTextColorActive;
    ci::TextBox mTextBox;
    po::scene::TextViewRef mTextView;
  };
}
