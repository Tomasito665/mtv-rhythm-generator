#pragma once

#include <poScene/View.h>
#include "TensionLineNode.h"
#include "MTVRhythmSpace.h"

namespace rg {
  typedef std::shared_ptr<class TensionLine> TensionLineRef;

  class TensionLine : public po::scene::View
  {
  public:
    static TensionLineRef create(
      int nodeCount, float nodeRadius = 12.5f, 
      ci::ColorA nodeBaseColor = ci::Color::hex(0x4ECDC4),
      ci::ColorA nodeBlimpColor = ci::Color::hex(0xFFA400),
      ci::ColorA lineColor = ci::Color::hex(0x2B706B)
    );

    static inline Tension yPos2Tension(float yPos, float height) { return 1.0f - (yPos / height); }
    static inline float tension2YPos(Tension tension, float height) { return (1.0f - tension) * height; }
    static inline int xPos2Step(float xPos, int nSteps, float width) { return (int)std::floor(xPos / width * nSteps); }
    static inline float step2XPos(int step, int nSteps, float width) { return (step + 0.5f) / nSteps * width; }

    void update() override;
    void draw() override;

    // when preserveEnvelope is set to true, the envelope of the tension line 
    // is preserved (nodes are linearly interpolated)
    void setNodeCount(int nodeCount, bool preserveEnvelope = false);
    int getNodeCount() const;
    void blimpNode(int nodeIx);

    void setEnabled(bool enabled);
    inline bool isEnabled() { return mIsEnabled; }

    // data must point to an array with N elements where N equals getNodeCount(), 
    // otherwise this call will result in undefined behaviour
    void setData(const Tension * const data);
    const Tension * const getData();
    void resetData();

    void setStep(int step, Tension value);
    Tension getStep(int step);

  private:
    explicit TensionLine(int nodeCount, float nodeRadius, ci::ColorA nodeBaseColor, ci::ColorA nodeBlimpColor, ci::ColorA lineColor);
    ci::Path2d mLinePath;
    std::vector<TensionLineNodeRef> mNodes;
    std::vector<Tension> mData;
    float mNodeRadius;
    bool mIsEnabled;
    ci::Color mNodeBaseColor;
    ci::Color mNodeBlimpColor;
    ci::Color mLineColor;
    // colors in disabled mode are the enabled ones in grayscale
    ci::Color mNodeBaseColorDisabled;
    ci::Color mNodeBlimpColorDisabled;
    ci::Color mLineColorDisabled;
  };
}
