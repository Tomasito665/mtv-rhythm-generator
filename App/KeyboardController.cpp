#include "KeyboardController.h"


rg::KeyboardController::KeyboardController()
{
}


rg::KeyboardController::~KeyboardController()
{
}

void rg::KeyboardController::keyDown(const KeyEvent& event)
{
  const int code = event.getCode();

  if (mActiveKeys.find(code) == mActiveKeys.end())
    mActiveKeys.insert(code);
  else
    return;

  std::unordered_map<int, OnKeyCallbackFunc>::iterator keyCallbackPair = mCallbacks.find(code);

  if (keyCallbackPair != mCallbacks.end()) {
    OnKeyCallbackFunc callback = keyCallbackPair->second;
    callback();
  }
}

void rg::KeyboardController::keyUp(const KeyEvent& event)
{
  const int code = event.getCode();
  if (mActiveKeys.find(code) != mActiveKeys.end()) {
    mActiveKeys.erase(code);
  }
}

void rg::KeyboardController::bind(int keyCode, OnKeyCallbackFunc callback)
{
  mCallbacks[keyCode] = callback;
}

void rg::KeyboardController::unbind(int keyCode)
{
  if (mCallbacks.find(keyCode) != mCallbacks.end()) {
    mCallbacks.erase(keyCode);
  }
}
