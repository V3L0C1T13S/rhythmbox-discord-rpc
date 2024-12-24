# Rhythmbox Discord RPC

Shows the music you're listening to in Rhythmbox on Discord.

## Building

Make sure to recurse submodules when cloning.

Ensure you install these dependencies:

* curl
* libpeas
* rhythmbox

Then run the following command:

```sh
meson setup build
ninja -C build
```
