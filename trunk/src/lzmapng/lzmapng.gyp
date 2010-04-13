{
  'targets': [
    {
      'target_name': 'xz_compress',
      'type': 'executable',
      'dependencies': [
        '<(DEPTH)/third_party/liblzma/liblzma.gyp:liblzma',
      ],
      'sources': [
        'xz_compress.c',
      ],
    },
  ],
}
