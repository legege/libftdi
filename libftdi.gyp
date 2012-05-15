{
  'targets': [
    {
      'target_name': 'libftdi',
      'type': 'static_library',

      'dependencies': [
        '../libusb/libusb.gyp:libusb'
      ],
      'sources': [
        'src/ftdi.c',
        'src/ftdi_stream.c'
      ],
      'include_dirs': [
        '.',
        'src'
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'src',
        ],
      },
    }
  ]
}
