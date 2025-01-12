#include "track.hpp"
#include <chrono>
#include <rhythmbox/plugins/rb-plugin-macros.h>

int MusicTrack::get_remaining_seconds() {
    auto current_time = std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();
    return current_time + duration - time;
}

MusicTrack::MusicTrack(RhythmDBEntry *entry) {
    title = rhythmdb_entry_get_string(entry, RHYTHMDB_PROP_TITLE);
    artist = rhythmdb_entry_get_string(entry, RHYTHMDB_PROP_ARTIST);
    album = rhythmdb_entry_get_string(entry, RHYTHMDB_PROP_ALBUM);
    duration = rhythmdb_entry_get_ulong(entry, RHYTHMDB_PROP_DURATION);
}
