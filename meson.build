project('rhythmbox-discord-rpc-v2', 'cpp')

cmake = import('cmake')

sources = [
    'plugin.cpp',
    'upload.cpp',
]

rhythmbox_dep = dependency('rhythmbox')
libpeas_dep = dependency('libpeas-gtk-1.0')
curl_dep = dependency('libcurl')

discord_rpc_opts = cmake.subproject_options()
discord_rpc_opts.add_cmake_defines({
    'BUILD_EXAMPLES': 'OFF',
    'CMAKE_POSITION_INDEPENDENT_CODE': 'ON',
})
discord_dep = cmake.subproject('discord-rpc', options: discord_rpc_opts).dependency('discord-rpc')

shared_module('rb-discord', sources, dependencies: [rhythmbox_dep, libpeas_dep, discord_dep, curl_dep])