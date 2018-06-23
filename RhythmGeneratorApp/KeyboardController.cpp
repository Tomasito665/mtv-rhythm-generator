#include "KeyboardController.h"



KeyboardController::KeyboardController()
{
}


KeyboardController::~KeyboardController()
{
}

void KeyboardController::keyDown(const KeyEvent& event)
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

void KeyboardController::keyUp(const KeyEvent& event)
{
  const int code = event.getCode();
  if (mActiveKeys.find(code) != mActiveKeys.end()) {
    mActiveKeys.erase(code);
  }
}

void KeyboardController::bind(int keyCode, OnKeyCallbackFunc callback)
{
  mCallbacks[keyCode] = callback;
}

void KeyboardController::unbind(int keyCode)
{
  if (mCallbacks.find(keyCode) != mCallbacks.end()) {
    mCallbacks.erase(keyCode);
  }
}
