#pragma once

#include <poScene/View.h>


namespace rg {
  typedef std::shared_ptr<class TensionLineNode> TensionLineNodeRef;

  class TensionLineNode : public po::scene::View
  {
  public:
    static TensionLineNodeRef create(
      int id, float radius = 12.5f,
      ci::Color defaultColor = ci::Color::hex(0xFF6B6B),
      ci::Color blimpColor = ci::Color::hex(0xFF8585)
    );

    void setup() override;
    void update() override;
    void blimp();

    void setBaseColor(const ci::Color& color);
    inline const ci::Color& getBaseColor() const { return mBaseColor; };
    void setBlimpColor(const ci::Color& color);
    inline const ci::Color& getBlimpColor() const { return mBlimpColor; }

  private:
    TensionLineNode(int id, float radius);
    int mId;
    float mRadius;
    bool mMouseHover;
    po::scene::ShapeViewRef mCircle;
    ci::Color mBaseColor;
    ci::Color mBlimpColor;
  };
}
