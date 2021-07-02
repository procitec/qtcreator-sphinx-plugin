=======================
qtcreator-sphinx-plugin
=======================
Plugin for QtCreator to improve editing of Sphinx RestructredText files.

The plugin is currently **Under Development**. Usage is possible, but on own risk.
If you have issues or improvments, feel free to create an issue.


Features
========

* Extends TextEditor with Toolbar for formating options
* Custom Sphinx highlighting
* *Preview* warnings by using `rstcheck <https://pypi.org/project/rstcheck/>`_
* Code completion for directive and roles (proof of concept, directive config later)
* Parser for Sphinx Warnings and Errors
* Configurable Sphinx Snippets
* File To HTML Result Linkage with View in right sidebar
* Quick preview during typing with `rst2html5 <https://pypi.org/project/rst2html5/>`_

For further details refer to `Usage documentation <doc/Usage.md>`_.

Build
=====

To build the plugin, you must compile QtCreator from source or get an appropriate version from 
`QtCreator Snapshots <https://download.qt.io/snapshots/qtcreator/>`_ or `QtCreator Releases <https://download.qt.io/official_releases/qtcreator>`_ .

The plugin could be build by using the "build_plugin.py" script provided by QtCreator.
A short wrapper script for this "build.py" is available in the project root.

After the build the output directory could be copied to the QtCreator directory.

Alternatively you can get a compiled plugin binary from the provided `Releases <https://github.com/procitec/qtcreator-sphinx-plugin/releases>`_.
Please ensure to have the identical QtCreator Version from these builds.
The Versions and Urls for Qt and QtCreator are provided with the build artefacts.

The master branch is build against snapshots from the master repository, the branches are build against dedicated offical QtCreator Releases.

Installation
============

Unzip the binarys to the QtCreator installation directory. The plugin consists of

* binary plugin shared object (lib directory)
* configuration files (share directory).

Activate the plugin in the QtCreator Plugin Settings
and restart QtCreator.

Dependencies
============

The plugin has runtime dependencies for `rstcheck <https://pypi.org/project/rstcheck/>`_  and `rst2html5 <https://pypi.org/project/rst2html5/>`_ if enabled.
An apropriate python executable must be configured to import *rstcheck* and *rst2html5* as a module from within the plugin.
Configuration of environment is not possible, the provided wrapper script requires import of the modules.
The script is located in the *share* directory of the plugin and could be adapted to your needs.

