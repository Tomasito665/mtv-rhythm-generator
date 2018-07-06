#pragma once

#include <cinder/Signals.h>
#include <poScene/ViewController.h>
#include "MultiSwitch.h"
#include "TimeSignature.h"

namespace rg {
  typedef std::shared_ptr<class ControlBar> ControlBarRef;
  typedef std::shared_ptr<class MultiSwitch<TimeSignature*>> TimeSignatureSwitchRef;
  typedef std::shared_ptr<class MultiSwitch<UnitRef>> StepUnitSwitchRef;

  class ControlBar : public po::scene::ViewController
  {
  public:
    static ControlBarRef create();

    void viewDidLoad() override;
    void update() override;

    ci::signals::Signal<void(const TimeSignature& ts)> sTimeSignatureRequest;
    ci::signals::Signal<void(UnitRef unit)> sStepUnitRequest;

    bool setSelectedTimeSignature(const TimeSignature& ts);
    bool setSelectedStepUnit(UnitRef unit);

    TimeSignature getSelectedTimeSignature() const;
    UnitRef getSelectedStepUnit() const;

    void setDisabled(bool disabled);

  protected:
    void onTimeSignatureOptionClicked(int optIx, TimeSignature * ts);
    void onStepUnitOptionClicked(int optIx, UnitRef unit);

  private:
    ControlBar();
    TimeSignature mTimeSignatures[3] = { TimeSignature(4, 4), TimeSignature(3, 4), TimeSignature(6, 8) };
    UnitRef mStepUnits[3] = { Unit::SEMIQUAVER, Unit::QUAVER, Unit::CROTCHET };
    po::scene::ShapeViewRef mBackground;
    TimeSignatureSwitchRef mTimeSignatureSwitch;
    StepUnitSwitchRef mStepUnitSwitch;
  };
}
