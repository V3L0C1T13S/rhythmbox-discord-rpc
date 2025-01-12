let pkgs = import <nixpkgs> {};
in pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    meson
    gcc
    ninja
    pkg-config
    cmake
    clang-tools
    mesonlsp
  ];

  buildInputs = with pkgs; [
    rhythmbox
    curl
    libsoup_3
    openssl

    # rhythmbox deps
    glib
    gtk3
    libpeas
    totem-pl-parser
    libgudev
    libgpod
    libmtp
    lirc
    brasero
    grilo
    gst_all_1.gstreamer
    gst_all_1.gst-plugins-base
    gst_all_1.gst-plugins-good
    gst_all_1.gst-plugins-bad
    gst_all_1.gst-plugins-ugly
    gst_all_1.gst-libav
  ];
  extraOutputsToInstall = [ "dev" ];
}
