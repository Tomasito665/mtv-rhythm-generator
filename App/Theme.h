#pragma once

#include <cinder/Color.h>

using ci::Color;

namespace rg {
  struct Theme {
    Color black = ci::Color(0, 0, 0);
    Color white = ci::Color(1, 1, 1);

    Color backgroundLight;
    Color backgroundBase;
    Color backgroundDark;

    Color primaryLight;
    Color primaryBase;
    Color primaryDark;

    Color secondaryLight;
    Color secondaryBase;
    Color secondaryDark;

    Color grayBase;
    Color grayDark;

    Color accent;

    static Theme& get()
    {
      static Theme theme;
      return theme;
    }

  private:
    Theme() {}
    Theme(const Theme&) = delete;
    void operator=(const Theme&) = delete;
  };
}
