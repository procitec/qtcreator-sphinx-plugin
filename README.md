# qtcreator-sphinx-plugin

Plugin for QtCreator to improvide editing of Sphinx RestructredText files

## Build

To build the plugin, you must compile QtCreator from Source or get an appropriate version from 
[QtCreator Snapshots](https://download.qt.io/snapshots/qtcreator/)

The plugin required the environment variables **QTC_SOURCE** and **QTC_BUILD** to be set (see [QtCreator Plugin Framework](https://doc-snapshots.qt.io/qtcreator-extending/first-plugin.html) ).

To build the plugin simply run qmake in the build directory, pointing to the root qtcreator-sphinx-plugin.pro file.

After build you can install the plugin using
*make install INSTALL_ROOT=dest* to install the plugin together with the configuration files.

Alternativly you can get a compiled plugin binary from the provided releases. Please ensure to have the identical QtCreator Version from these builds.

## Installation

Unzip the binarys to the QtCreator installation directory. The plugin consists of

* binary plugin shared object (lib directory)
* configuration files (share directory).

Activate the plugin in the QtCreator Plugin Settings
and restart QtCreator.


## Dependencies

The plugin has runtime dependencies for [rstcheck](https://pypi.org/project/rstcheck/) if enabled. An apropriate python executable must be configured to run *rstcheck* as a module from within the plugin.


## Usage

Refer to [Usage Documentation](doc/Usage.md).






