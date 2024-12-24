# Rhythmbox Discord RPC

Shows the music you're listening to in Rhythmbox on Discord.

## Features

* Album art displayed in Discord
* Fast, like, **really** fast

![Screenshot_20241224_171920](https://github.com/user-attachments/assets/3047220f-4c1b-4090-b831-ce1e5d2fe39a)

## Building

Make sure to recurse submodules when cloning.

Ensure you install these dependencies:

* curl
* libpeas
* rhythmbox

Note for Nix users: this repository already comes with a default.nix - if you use Nix, simply run `nix-shell` to get a working dev environment.

Then run the following commands:

```sh
meson setup build
ninja -C build
```

Once the project has been built, copy the plugin files into your Rhythmbox plugins directory:

```sh
cp ./build/librb-discord.so ~/.local/share/rhythmbox/plugins/librb-discord.so
cp rhythmbox-discord-v2.plugin ~/.local/share/rhythmbox/plugins/rhythmbox-discord-v2.plugin
```

Now open Rhythmbox's preferences, and enable the plugin inside the Plugins tab.