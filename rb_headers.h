#pragma once

#include <glib.h>

typedef void *RBShell;
typedef void *RBShellPlayer;
typedef void *RhythmDB;
typedef void *RhythmDBEntry;
typedef void *RhythmboxArtStore;
typedef void *RBExtDBKey;
typedef void *RBExtDBRequestCallback;

typedef enum {
    RHYTHMDB_PROP_TYPE = 0,
    RHYTHMDB_PROP_ENTRY_ID,
    RHYTHMDB_PROP_TITLE,
    RHYTHMDB_PROP_GENRE,
    RHYTHMDB_PROP_ARTIST,
    RHYTHMDB_PROP_ALBUM,
    RHYTHMDB_PROP_TRACK_NUMBER,
    RHYTHMDB_PROP_TRACK_TOTAL,
    RHYTHMDB_PROP_DISC_NUMBER,
    RHYTHMDB_PROP_DISC_TOTAL,
    RHYTHMDB_PROP_DURATION,
    RHYTHMDB_PROP_FILE_SIZE,
    RHYTHMDB_PROP_LOCATION,
    RHYTHMDB_PROP_MOUNTPOINT,
    RHYTHMDB_PROP_MTIME,
    RHYTHMDB_PROP_FIRST_SEEN,
    RHYTHMDB_PROP_LAST_SEEN,
    RHYTHMDB_PROP_RATING,
    RHYTHMDB_PROP_PLAY_COUNT,
    RHYTHMDB_PROP_LAST_PLAYED,
    RHYTHMDB_PROP_BITRATE,
    RHYTHMDB_PROP_DATE,
    RHYTHMDB_PROP_TRACK_GAIN, /* obsolete */
    RHYTHMDB_PROP_TRACK_PEAK, /* obsolete */
    RHYTHMDB_PROP_ALBUM_GAIN, /* obsolete */
    RHYTHMDB_PROP_ALBUM_PEAK, /* obsolete */
    RHYTHMDB_PROP_MEDIA_TYPE,
    RHYTHMDB_PROP_TITLE_SORT_KEY,
    RHYTHMDB_PROP_GENRE_SORT_KEY,
    RHYTHMDB_PROP_ARTIST_SORT_KEY,
    RHYTHMDB_PROP_ALBUM_SORT_KEY,
    RHYTHMDB_PROP_TITLE_FOLDED,
    RHYTHMDB_PROP_GENRE_FOLDED,
    RHYTHMDB_PROP_ARTIST_FOLDED,
    RHYTHMDB_PROP_ALBUM_FOLDED,
    RHYTHMDB_PROP_LAST_PLAYED_STR,
    RHYTHMDB_PROP_HIDDEN,
    RHYTHMDB_PROP_PLAYBACK_ERROR,
    RHYTHMDB_PROP_FIRST_SEEN_STR,
    RHYTHMDB_PROP_LAST_SEEN_STR,

    /* synthetic properties */
    RHYTHMDB_PROP_SEARCH_MATCH,
    RHYTHMDB_PROP_YEAR,
    RHYTHMDB_PROP_KEYWORD, /**/

    /* Podcast properties */
    RHYTHMDB_PROP_STATUS,
    RHYTHMDB_PROP_DESCRIPTION,
    RHYTHMDB_PROP_SUBTITLE,
    RHYTHMDB_PROP_SUMMARY, /* obsolete */
    RHYTHMDB_PROP_LANG,
    RHYTHMDB_PROP_COPYRIGHT,
    RHYTHMDB_PROP_IMAGE,
    RHYTHMDB_PROP_POST_TIME,
    RHYTHMDB_PROP_PODCAST_GUID,

    RHYTHMDB_PROP_MUSICBRAINZ_TRACKID,
    RHYTHMDB_PROP_MUSICBRAINZ_ARTISTID,
    RHYTHMDB_PROP_MUSICBRAINZ_ALBUMID,
    RHYTHMDB_PROP_MUSICBRAINZ_ALBUMARTISTID,
    RHYTHMDB_PROP_ARTIST_SORTNAME,
    RHYTHMDB_PROP_ALBUM_SORTNAME,

    RHYTHMDB_PROP_ARTIST_SORTNAME_SORT_KEY,
    RHYTHMDB_PROP_ARTIST_SORTNAME_FOLDED,
    RHYTHMDB_PROP_ALBUM_SORTNAME_SORT_KEY,
    RHYTHMDB_PROP_ALBUM_SORTNAME_FOLDED,

    RHYTHMDB_PROP_COMMENT,
    RHYTHMDB_PROP_ALBUM_ARTIST,
    RHYTHMDB_PROP_ALBUM_ARTIST_SORT_KEY,
    RHYTHMDB_PROP_ALBUM_ARTIST_FOLDED,
    RHYTHMDB_PROP_ALBUM_ARTIST_SORTNAME,
    RHYTHMDB_PROP_ALBUM_ARTIST_SORTNAME_SORT_KEY,
    RHYTHMDB_PROP_ALBUM_ARTIST_SORTNAME_FOLDED,
    RHYTHMDB_PROP_BPM,

    RHYTHMDB_PROP_COMPOSER,
    RHYTHMDB_PROP_COMPOSER_SORT_KEY,
    RHYTHMDB_PROP_COMPOSER_FOLDED,
    RHYTHMDB_PROP_COMPOSER_SORTNAME,
    RHYTHMDB_PROP_COMPOSER_SORTNAME_SORT_KEY,
    RHYTHMDB_PROP_COMPOSER_SORTNAME_FOLDED,

    RHYTHMDB_NUM_PROPERTIES
} RhythmDBPropType;

extern "C" RhythmboxArtStore *rb_ext_db_new(const char *name);
extern "C" const char *rhythmdb_entry_get_string(RhythmDBEntry *entry,
                                                 RhythmDBPropType prop);
extern "C" ulong rhythmdb_entry_get_ulong(RhythmDBEntry *entry,
                                          RhythmDBPropType prop);
extern "C" RBExtDBKey *rhythmdb_entry_create_ext_db_key(RhythmDBEntry *entry,
                                                        RhythmDBPropType prop);
extern "C" void rb_ext_db_request(RhythmDBEntry *entry, RBExtDBKey *key,
                                  RBExtDBRequestCallback cb, void *plugin,
                                  void *cb_2);
extern "C" void rb_ext_db_key_free(RBExtDBKey *key);
extern "C" bool rhythmdb_entry_matches_ext_db_key(RhythmDB *db,
                                                  RhythmDBEntry *entry,
                                                  RBExtDBKey *key);
extern "C" void rb_shell_player_get_playing_time(RBShellPlayer *player,
                                                 guint *time, GError *error);
