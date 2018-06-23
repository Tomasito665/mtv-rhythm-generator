#pragma once

#include <cinder/app/App.h>
#include <unordered_set>
#include <unordered_map>
#include <functional>

using namespace ci;
using namespace ci::app;
typedef std::function<void()> OnKeyCallbackFunc;

class KeyboardController
{
public:
  KeyboardController();
  virtual ~KeyboardController();

  // events
  void keyDown(const KeyEvent& event);
  void keyUp(const KeyEvent& event);

  void bind(int keyCode, OnKeyCallbackFunc callback);  // binds an action to a key
  void unbind(int keyCode);  // unbinds whatever action is bound to the given key

private:
  std::unordered_set<int> mActiveKeys;
  std::unordered_map<int, OnKeyCallbackFunc> mCallbacks;
};
