#pragma once

#include <poScene/View.h>

namespace rg {
  class ToggleButton : public po::scene::View
  {
  public:
    void setup() override;
    void update() override;

    void setActive(bool active);
    inline bool isActive() const { return mActive; }

    enum Highlight { ON_MOUSE_HOVER, ON, OFF };
    void highlight(Highlight highlight);

    void setDisabled(bool disabled);
    inline bool isDisabled() const { return mDisabled; }

    inline void setColorBase(const ci::Color& color) { mColorBase = color; }
    inline void setColorBaseHover(const ci::Color& color) { mColorBaseHover = color; }
    inline void setColorActive(const ci::Color& color) { mColorActive = color; }
    inline void setColorActiveHover(const ci::Color& color) { mColorActiveHover = color; }

    inline ci::Color getColorBase() const { return mColorBase; }
    inline ci::Color getColorBaseHover() const { return mColorBaseHover; }
    inline ci::Color getColorActive() const { return mColorActive; }
    inline ci::Color getColorActiveHover() const { return mColorActiveHover; }

  protected:
    ToggleButton();

    virtual po::scene::ShapeViewRef createToggleShape() = 0;
    virtual void onClick() = 0;

    void onMouseMove(po::scene::MouseEvent& event);
    void onMouseDownInside(po::scene::MouseEvent& event);
    void onMouseUp(po::scene::MouseEvent& event);

  private:
    Highlight mHighlight;
    bool mDisabled;
    bool mActive;
    bool mMouseDown;
    bool mMouseHover;
    ci::Color mColorBase;
    ci::Color mColorBaseHover;
    ci::Color mColorActive;
    ci::Color mColorActiveHover;
    po::scene::ShapeViewRef mShape;
  };
}
