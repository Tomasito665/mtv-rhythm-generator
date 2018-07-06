#pragma once

#include <bitset>
#include <vector>
#include "Types.h"
#include "Unit.h"
#include "TimeSignature.h"

enum MusicalEventType {
  NOTE = 1,
  REST = 0,
  TIED_NOTE = -1
};

struct MusicalEvent
{
  MusicalEventType type;
  int position;
  int duration;

  MusicalEvent(MusicalEventType type, int position, int duration) :
    type(type), position(position), duration(duration) {}
  
  MusicalEvent(const MusicalEvent &other) :
    type(other.type), position(other.position), duration(other.duration) {}

  inline int getTrailingPosition() const { return position + duration - 1; }

  inline bool operator==(const MusicalEvent &other) const {
    return type == other.type
      && position == other.position
      && duration == other.duration;
  }
};

class RhythmPattern;
typedef std::vector<MusicalEvent> MusicalEventList;
typedef std::shared_ptr<class RhythmPattern> RhythmPatternRef;

// monophonic rhythm with a duration of one measure
class RhythmPattern
{
public:
  explicit RhythmPattern(
    const TimeSignature& ts = TimeSignature(4, 4),
    UnitRef stepUnit = Unit::QUAVER, 
    PatternId pattern = EMPTY_RHYTHM_PATTERN  // NOTE: LSB in pattern id represents first step (so, pattern from right to left)
  );
  virtual ~RhythmPattern();

  // converts the given binary pattern to a pattern id (vector must have a size less than MAX_N_STEPS)
  static PatternId vector2PatternId(std::vector<bool> patternVec);
  // converts the given pattern id to a binary vector
  static std::vector<bool> patternId2Vector(PatternId patternId, int nSteps);
  // fills the given vector with the binary representation of the given pattern
  static void patternId2Vector(PatternId patternId, std::vector<bool>::iterator from, std::vector<bool>::iterator to);

  // clear all onsets
  void reset();
  // set time signature and clear all onsets
  void reset(const TimeSignature& ts);
  // set step unit and clear all onsets
  void reset(UnitRef stepUnit);
  // set time signature and step unit and clear all onsets
  void reset(const TimeSignature& ts, UnitRef stepUnit);
  // updates the steps in this rhythm pattern according to the given id
  void update(PatternId patternId);
  // set the given step (true for onset, false for rest)
  std::bitset<MAX_N_STEPS>::reference operator[] (size_t index);
  // returns true if there's an onset on the given step; false otherwise
  bool operator[] (size_t index) const;
  // returns true if this rhythm's pattern time signature, step unit and pattern are equal to the other's
  bool operator==(const RhythmPattern &other) const;
  // returns true if == returns false
  bool operator!=(const RhythmPattern &other) const;
  // toggles the given step (converts onset into non onset and vice-versa)
  void toggle(size_t index);
  // sets the rhythm pattern given its id (LSB is first step)
  void setPatternId(PatternId pattern);
  // returns rhythm pattern id for current onsets (this may change during the lifetime of this object)
  PatternId getPatternId() const;
  // converts this rhythm pattern to musical events (notes, rests and tied notes) and returns the events as a vector
  std::vector<MusicalEvent> asMusicalEvents(bool cyclic = true, bool trimDurationsToBeat = true) const;
  
  // getters
  inline int getNSteps() const { return mNSteps; }
  inline const TimeSignature& getTimeSignature() const { return mTs; }
  inline UnitRef getStepUnit() const { return mStepUnit; }

protected:
  void checkStepIndex(size_t index) const;
  void checkNSteps() const;
  void updateNSteps();

private:
  TimeSignature mTs;
  UnitRef mStepUnit;
  int mNSteps;
  std::bitset<MAX_N_STEPS> mPattern;
};
