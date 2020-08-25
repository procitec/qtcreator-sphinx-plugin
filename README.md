# qtcreator-sphinx-plugin

Plugin for QtCreator to improvide editing of Sphinx RestructredText files.

The plugin is currently **Under Development**. Usage is possible, but on own risk.
If you have issues or improvments, feel free to create an issue.


## Features

* Extends TextEditor with Toolbar for formating options
* Custom Sphinx highlighting
* *Preview* warnings by using [rstcheck](https://pypi.org/project/rstcheck/)
* Code completion for directive and roles (proof of concept, directive config later)
* Parser for Sphinx Warnings and Errors
* Configurable Sphinx Snippets
* File To HTML Result Linkage

For further details refer to [Usage documentation](doc/Usage.md).

## Build

To build the plugin, you must compile QtCreator from Source or get an appropriate version from 
[QtCreator Snapshots](https://download.qt.io/snapshots/qtcreator/)

The plugin required the environment variables **QTC_SOURCE** and **QTC_BUILD** to be set
(see [QtCreator Plugin Framework](https://doc-snapshots.qt.io/qtcreator-extending/first-plugin.html) ).

To build the plugin simply run qmake in the build directory, pointing to the root qtcreator-sphinx-plugin.pro file
(an out-of-source build is recommended).

After build you can install the plugin with *make install INSTALL_ROOT=dest* to install the plugin together with the configuration files.

Alternatively you can get a compiled plugin binary from the provided releases. Please ensure to have the identical QtCreator Version from these builds.
The Versions and Urls for Qt and QtCreator are provided with the build artefact.

## Installation

Unzip the binarys to the QtCreator installation directory. The plugin consists of

* binary plugin shared object (lib directory)
* configuration files (share directory).

Activate the plugin in the QtCreator Plugin Settings
and restart QtCreator.

## Dependencies

The plugin has runtime dependencies for [rstcheck](https://pypi.org/project/rstcheck/) if enabled.
An apropriate python executable must be configured to import *rstcheck* as a module from within the plugin.
Configuration of environment is not possible, the provided wrapper script requires import of rstcheck.
The script is located in the *share* directory of the plugin and could be adapted to your needs.

