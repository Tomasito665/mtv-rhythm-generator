#include "TensionLine.h"
#include <cinder/gl/gl.h>
#include <cinder/ChanTraits.h>
#include "Utils.h"

rg::TensionLineRef rg::TensionLine::create(int nodeCount, float nodeRadius, ci::ColorA nodeBaseColor, ci::ColorA nodeBlimpColor, ci::ColorA lineColor)
{
  TensionLineRef ref(new TensionLine(nodeCount, nodeRadius, nodeBaseColor, nodeBlimpColor, lineColor));
  return ref;
}

void rg::TensionLine::update()
{
  const int N = mData.size();
  const float height = getHeight();
  const float width = getWidth();

  // if node count updated
  if (mNodes.size() != N) {
    auto nodeIt = mNodes.cbegin();

    // remove old node views
    while (nodeIt != mNodes.cend()) {
      removeSubview(*nodeIt);
      nodeIt = mNodes.erase(nodeIt);
    }
    
    // add new node views
    for (int i = 0; i < N; ++i) {
      TensionLineNodeRef node = TensionLineNode::create(i, mNodeRadius, 
        mNodeBaseColor, mNodeBlimpColor);
      node->setOffset(step2XPos(i, N, width), 0);
      mNodes.push_back(node);
      addSubview(node);
    }
  }

  const ci::Color& nodeBaseColor = mIsEnabled ? mNodeBaseColor : mNodeBaseColorDisabled;
  const ci::Color& nodeBlimpColor = mIsEnabled ? mNodeBlimpColor : mNodeBlimpColorDisabled;

  const float cyclicTensionDelta = mData.front() - mData.back();
  const float cyclicTensionOffset = cyclicTensionDelta * 0.5f;
  mLinePath.clear();

  // draw cyclic line from the last step of the imaginary previous measure
  mLinePath.moveTo(0.0f, (1.0f - (mData.front() - cyclicTensionOffset)) * height);

  // set vertical node position and line path points
  for (int i = 0; i < N; ++i) {
    Tension t = mData[i];
    TensionLineNodeRef node = mNodes[i];
    node->setBaseColor(nodeBaseColor);
    node->setBlimpColor(nodeBlimpColor);
    node->setOffset(node->getOffset().x, tension2YPos(t, height));
    mLinePath.lineTo(node->getOffset());
  }

  // draw cyclic line to first step of the imaginary next measure
  mLinePath.lineTo(getWidth(), (1.0f - (mData.back() + cyclicTensionOffset)) * height);
}

void rg::TensionLine::draw()
{
  ci::gl::enableAlphaBlending();
  ci::gl::lineWidth(mNodeRadius * 0.5f);
  ci::gl::color(mLineColor);
  ci::gl::draw(mLinePath);
  po::scene::View::draw();
}

void rg::TensionLine::setNodeCount(int N, bool preserveEnvelope)
{
  if (!preserveEnvelope) {
    mData.resize(N);
    return;
  }

  const int oldN = getNodeCount();
  std::vector<Tension> oldData = std::move(mData);
  mData.resize(N);

  for (int i = 0; i < N; ++i) {
    const double pos = (double)i / N * oldN;
    const int ixLo = (int)pos;
    const int ixHi = (oldN == 0) ? 0 : (ixLo + 1) % oldN;
    const Tension delta = oldData[ixHi] - oldData[ixLo];
    const double ratio = pos - ixLo;
    mData[i] = oldData[ixLo] + delta * ratio;
  }
}

int rg::TensionLine::getNodeCount() const
{
  return mNodes.size();
}

void rg::TensionLine::blimpNode(int nodeIx)
{
  mNodes[nodeIx]->blimp();
}

void rg::TensionLine::setEnabled(bool enabled)
{
  mIsEnabled = enabled;
}

void rg::TensionLine::setData(const Tension * const data)
{
  const int N = mData.size();
  for (int i = 0; i < N; ++i) 
    mData[i] = data[i];
}

const Tension * const rg::TensionLine::getData()
{
  return &mData[0];
}

void rg::TensionLine::resetData()
{
  for (Tension& tension : mData)
    tension = 0.0f;
}

void rg::TensionLine::setStep(int step, Tension value)
{
  mData[step] = value;
}

Tension rg::TensionLine::getStep(int step)
{
  return mData[step];
}

rg::TensionLine::TensionLine(int nodeCount, float nodeRadius, ci::ColorA nodeBaseColor, ci::ColorA nodeBlimpColor, ci::ColorA lineColor) :
  mNodes(std::vector<TensionLineNodeRef>(0)),  // <- force node reallocation at first update()
  mData(std::vector<Tension>(nodeCount)),
  mNodeRadius(nodeRadius),
  mNodeBaseColor(nodeBaseColor),
  mNodeBaseColorDisabled(toGrayscale(nodeBaseColor)),
  mNodeBlimpColor(nodeBlimpColor),
  mNodeBlimpColorDisabled(toGrayscale(nodeBlimpColor)),
  mLineColor(lineColor),
  mLineColorDisabled(toGrayscale(lineColor))
{
}
