#include <rhythmbox/plugins/rb-plugin-macros.h>
#include "rb_headers.h"
#include <iostream>
#include <mutex>
#include <chrono>
#include "discord_rpc.h"
#include "track.hpp"
#include <thread>
#include <atomic>
#include <string>
#include "upload.hpp"

#define CLIENT_ID "1212075104486301696"

#define RB_TYPE_DISCORD_PLUGIN (rb_discord_plugin_get_type())
#define RB_DISCORD_PLUGIN(o) \
    (G_TYPE_CHECK_INSTANCE_CAST((o), RB_TYPE_DISCORD_PLUGIN, RBDiscordPlugin))
#define RB_DISCORD_PLUGIN_CLASS(k) \
    (G_TYPE_CHECK_CLASS_CAST((k), RB_TYPE_DISCORD_PLUGIN, RBDiscordPluginClass))
#define RB_IS_DISCORD_PLUGIN(o) \
    (G_TYPE_CHECK_INSTANCE_TYPE((o), RB_TYPE_DISCORD_PLUGIN))
#define RB_IS_DISCORD_PLUGIN_CLASS(k) \
    (G_TYPE_CHECK_CLASS_TYPE((k), RB_TYPE_DISCORD_PLUGIN))
#define RB_DISCORD_PLUGIN_GET_CLASS(o) \
    (G_TYPE_INSTANCE_GET_CLASS((o), RB_TYPE_DISCORD_PLUGIN, RBDiscordPluginClass))

struct RBDiscordPlugin
{
    PeasExtensionBase parent;
    RBShell *shell_player;
    RhythmDB *db;
    RhythmboxArtStore *art_store;
    RhythmDBEntry *playing_entry = nullptr;
    std::atomic_bool running;
};

struct RBDiscordPluginClass
{
    PeasExtensionBaseClass parent_class;
};

RB_DEFINE_PLUGIN(RB_TYPE_DISCORD_PLUGIN, RBDiscordPlugin, rb_discord_plugin, )

static bool rpc_connected;
static std::mutex presence_mutex;

static std::string details = "";
static std::string state = "";
static std::string large_image_key = "";
static std::string large_image_text = "";
static std::string small_image_key = "";
static std::string small_image_text = "";
static long start_timestamp = 0;
static long end_timestamp = 0;

constexpr float update_interval = 0.5f;
static std::chrono::time_point last_update = std::chrono::steady_clock::now();

std::thread discord_thread;

void discord_rpc_update()
{
    if (std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - last_update).count() < update_interval)
        return;
    last_update = std::chrono::steady_clock::now();

    if (!rpc_connected)
    {
        Discord_RunCallbacks();
        return;
    }

    std::scoped_lock guard(presence_mutex);

    DiscordRichPresence discord_presence;
    memset(&discord_presence, 0, sizeof(discord_presence));
    discord_presence.details = details.c_str();
    discord_presence.state = state.c_str();
    discord_presence.largeImageKey = large_image_key.c_str();
    discord_presence.largeImageText = large_image_text.c_str();
    discord_presence.smallImageKey = small_image_key.c_str();
    discord_presence.smallImageText = small_image_text.c_str();
    discord_presence.startTimestamp = start_timestamp;
    discord_presence.endTimestamp = end_timestamp;
    Discord_UpdatePresence(&discord_presence);

    Discord_RunCallbacks();
}

void discord_rpc_loop(const RBDiscordPlugin *plugin)
{
    while (plugin->running.load())
    {
        discord_rpc_update();
    }
}

void set_discord_music(MusicTrack music)
{
    std::scoped_lock guard(presence_mutex);

    auto current_time = time(NULL);
    details = music.title;
    state = "";
    large_image_key = "logo";
    large_image_text = "Rhythmbox";
    small_image_key = "";
    small_image_text = "";
    start_timestamp = current_time;
    end_timestamp = current_time + music.duration;
}

void discord_rpc_init(RBDiscordPlugin *plugin)
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = [](const DiscordUser *user)
    {
        std::cout << "connected to user " << user->userId << std::endl;
        rpc_connected = true;
    };
    handlers.disconnected = [](int errcode, const char *message)
    {
        std::cout << "disconnected (" << errcode << ": " << message << ")";
        rpc_connected = false;
    };
    handlers.errored = [](int errcode, const char *message)
    {
        std::cout << "error (" << errcode << ": " << message << std::endl;
    };
    handlers.joinGame = [](const char *) {};
    handlers.spectateGame = [](const char *) {};
    handlers.joinRequest = [](const DiscordUser *) {};
    Discord_Initialize(CLIENT_ID, &handlers, 0, nullptr);
    plugin->running.store(true);
    discord_thread = std::thread(discord_rpc_loop, plugin);
}

void discord_rpc_destroy()
{
    rpc_connected = false;
    Discord_ClearPresence();
    Discord_RunCallbacks();
    Discord_Shutdown();
}

MusicTrack get_song_entry_data(RhythmDBEntry *entry)
{
    MusicTrack data = {
        .title = rhythmdb_entry_get_string(entry, RHYTHMDB_PROP_TITLE),
        .artist = rhythmdb_entry_get_string(entry, RHYTHMDB_PROP_ARTIST),
        .album = rhythmdb_entry_get_string(entry, RHYTHMDB_PROP_ALBUM),
        .duration = rhythmdb_entry_get_ulong(entry, RHYTHMDB_PROP_DURATION)};

    return data;
}

static void rb_discord_plugin_init(RBDiscordPlugin *plugin) {};

void upload_and_set_art_async(std::string filename)
{
    std::scoped_lock guard(presence_mutex);
    auto url = uploadFile(filename);
    large_image_key = url;
}

static void art_cb(RBExtDBKey *key, RBExtDBKey *store_key, const char *filename,
                   GValue *data, RBDiscordPlugin *self)
{
    RhythmDBEntry *entry = self->playing_entry;

    if (entry == nullptr)
        return;

    if (rhythmdb_entry_matches_ext_db_key(self->db, entry, store_key))
    {
        std::thread art_thread(upload_and_set_art_async, std::string(filename));
        art_thread.detach();
    }
}

void get_art(RBDiscordPlugin *self, RhythmDBEntry *entry)
{
    RBExtDBKey *key =
        rhythmdb_entry_create_ext_db_key(entry, RHYTHMDB_PROP_ALBUM);
    rb_ext_db_request(self->art_store, key, (RBExtDBRequestCallback)art_cb,
                      g_object_ref(self), (void *)g_object_unref);
    rb_ext_db_key_free(key);
}

static void playing_entry_changed_cb(RBShellPlayer *player,
                                     RhythmDBEntry *entry,
                                     RBDiscordPlugin *self)
{
    self->playing_entry = entry;
    if (entry == nullptr)
        return;
    auto data = get_song_entry_data(entry);

    set_discord_music(data);

    get_art(self, entry);
}

static void playing_changed_cb(RBShellPlayer *player, gboolean playing,
                               RBDiscordPlugin *self)
{
    if (self->playing_entry != nullptr)
    {
        // TODO
    }
}

static void impl_activate(PeasActivatable *bplugin)
{
    RBDiscordPlugin *plugin;
    RBShell *shell;

    plugin = RB_DISCORD_PLUGIN(bplugin);
    g_object_get(plugin, "object", &shell, NULL);
    g_object_get(shell, "shell-player", &plugin->shell_player, "db", &plugin->db,
                 NULL);

    g_signal_connect_object(plugin->shell_player, "playing-song-changed",
                            G_CALLBACK(playing_entry_changed_cb), plugin, G_CONNECT_DEFAULT);
    g_signal_connect_object(plugin->shell_player, "playing-changed",
                            G_CALLBACK(playing_changed_cb), plugin, G_CONNECT_DEFAULT);

    plugin->art_store = rb_ext_db_new("album-art");

    g_object_unref(shell);

    discord_rpc_init(plugin);
}

static void impl_deactivate(PeasActivatable *bplugin)
{
    RBDiscordPlugin *self = RB_DISCORD_PLUGIN(bplugin);
    self->running.store(false);
    discord_thread.join();
    discord_rpc_destroy();
}

extern "C" G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module)
{
    rb_discord_plugin_register_type(G_TYPE_MODULE(module));
    peas_object_module_register_extension_type(module, PEAS_TYPE_ACTIVATABLE,
                                               RB_TYPE_DISCORD_PLUGIN);
}
