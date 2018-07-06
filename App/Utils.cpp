#include "Utils.h"
#include <cinder/ChanTraits.h>

ci::Color toGrayscale(const ci::Color& color)
{
  const float l = ci::CHANTRAIT<float>::grayscale(color.r, color.g, color.b);
  return ci::Color(l, l, l);
}
