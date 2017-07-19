# nrfutil

This directory will house batch files etc to convert the hex files to zip files



D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil --help
Usage: nrfutil [OPTIONS] COMMAND [ARGS]...

Options:
  --verbose  Show verbose information
  --help     Show this message and exit.

Commands:
  dfu      This set of commands support Nordic DFU OTA...
  keys     Generate keys for signing or generate public keys
  version  Displays nrf utility version.

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu --help
Usage: nrfutil dfu [OPTIONS] COMMAND [ARGS]...

  This set of commands support Nordic DFU OTA package generation for
  distribution to applications and serial DFU.

Options:
  --help  Show this message and exit.

Commands:
  genpkg  Generate a package for distribution to Apps supporting Nordic DFU
          OTA
  serial  Program a device with bootloader that support serial DFU

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --help
Usage: nrfutil dfu genpkg [OPTIONS] ZIPFILE

  Generate a zipfile package for distribution to Apps supporting Nordic DFU
  OTA. The application, bootloader and softdevice files are converted to
  .bin if it is a .hex file. For more information on the generated init
  packet see:
  http://developer.nordicsemi.com/nRF51_SDK/doc/7.2.0/s110/html/a00065.html

Options:
  --application TEXT              The application firmware file
  --application-version INT OR NONE
                                  Application version, default: 0xFFFFFFFF
  --bootloader TEXT               The bootloader firmware file
  --dev-revision INT OR NONE      Device revision, default: 0xFFFF
  --dev-type INT OR NONE          Device type, default: 0xFFFF
  --dfu-ver FLOAT                 DFU packet version to use, default: 0.5
  --sd-req TEXT OR NONE           SoftDevice requirement. A list of
                                  SoftDevice versions (1 or more)of which one
                                  is required to be present on the target
                                  device.Example: --sd-req 0x4F,0x5A.
                                  Default: 0xFFFE.
  --softdevice TEXT               The SoftDevice firmware file
  --key-file PATH                 Signing key (pem fomat)
  --help                          Show this message and exit.

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>
D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --application  --help
Usage: nrfutil dfu genpkg [OPTIONS] ZIPFILE

Error: Missing argument "zipfile".None

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --application  app.hex aapppp.zip
Zip created at aapppp.zip

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --application  tuds_s130_dfu2_build1.00.hex  tuds_s130_dfu2
_build1.00.zip
Zip created at tuds_s130_dfu2_build1.00.zip

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --application  tuds_s130_dfu2_build1.01.hex  tuds_s130_dfu2
_build1.01.zip
Zip created at tuds_s130_dfu2_build1.01.zip

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --softdevice s130_nrf51_2.0.0_softdevice.hex s130_nrf51_2.0
.0_softdevice.zip
Zip created at s130_nrf51_2.0.0_softdevice.zip

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --help
Usage: nrfutil dfu genpkg [OPTIONS] ZIPFILE

  Generate a zipfile package for distribution to Apps supporting Nordic DFU
  OTA. The application, bootloader and softdevice files are converted to
  .bin if it is a .hex file. For more information on the generated init
  packet see:
  http://developer.nordicsemi.com/nRF51_SDK/doc/7.2.0/s110/html/a00065.html

Options:
  --application TEXT              The application firmware file
  --application-version INT OR NONE
                                  Application version, default: 0xFFFFFFFF
  --bootloader TEXT               The bootloader firmware file
  --dev-revision INT OR NONE      Device revision, default: 0xFFFF
  --dev-type INT OR NONE          Device type, default: 0xFFFF
  --dfu-ver FLOAT                 DFU packet version to use, default: 0.5
  --sd-req TEXT OR NONE           SoftDevice requirement. A list of
                                  SoftDevice versions (1 or more)of which one
                                  is required to be present on the target
                                  device.Example: --sd-req 0x4F,0x5A.
                                  Default: 0xFFFE.
  --softdevice TEXT               The SoftDevice firmware file
  --key-file PATH                 Signing key (pem fomat)
  --help                          Show this message and exit.

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>
D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>
D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>nrfutil dfu genpkg --bootloader dfu_s130_dfu2.hex dfu_s130_dfu2.zip
Zip created at dfu_s130_dfu2.zip

D:\j\proj\nrf\nRF5_SDK_11.0.0_89a8197\s130v11\nrfutil>
