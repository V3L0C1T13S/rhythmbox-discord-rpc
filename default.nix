let pkgs = import <nixpkgs> {};
in pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    meson
    gcc
    ninja
    pkg-config
    cmake
  ];

  buildInputs = with pkgs; [
    rhythmbox

    glib
    gtk3
    libsoup_3
    libpeas
    totem-pl-parser
    libgudev
    libgpod
    libmtp
    lirc
    brasero
    grilo

    libxml2

    gst_all_1.gstreamer
    gst_all_1.gst-plugins-base
    gst_all_1.gst-plugins-good
    gst_all_1.gst-plugins-bad
    gst_all_1.gst-plugins-ugly
    gst_all_1.gst-libav

    discord-rpc
    curl
  ];
  extraOutputsToInstall = [ "dev" ];
}