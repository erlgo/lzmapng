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
    {
      'target_name': 'png_optimizer',
      'type': '<(library)',
      'dependencies': [
        '<(DEPTH)/third_party/libpng/libpng.gyp:libpng',
        '<(DEPTH)/third_party/optipng/optipng.gyp:opngreduc',
        '<(DEPTH)/third_party/optipng/optipng.gyp:pngxrgif',
      ],
      'sources': [
        'gif_reader.cc',
        'png_optimizer.cc',
      ],
      'include_dirs': [
        '<(DEPTH)',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<(DEPTH)',
        ],
        'defines': [
          'PAGESPEED_PNG_OPTIMIZER_GIF_READER'
        ],
      },
      'defines': [
        'PAGESPEED_PNG_OPTIMIZER_GIF_READER'
      ]
    },
    {
      'target_name': 'optimize_image_bin',
      'type': 'executable',
      'dependencies': [
        'png_optimizer',
      ],
      'sources': [
        'optimize_image.cc',
      ],
    },
  ],
}
