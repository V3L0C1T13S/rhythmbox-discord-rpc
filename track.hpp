#pragma once

#include <string>
#include <optional>
#include "rb_headers.h"

class MusicTrack {
public:
  std::string title;
  std::string artist;
  std::string album;
  std::string art_url;
  guint time = 0;
  int duration;
  bool playing = true;

  int get_remaining_seconds();
  MusicTrack(RhythmDBEntry *entry);
private:
};
