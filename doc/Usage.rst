=====
Usage
=====

The plugin provides functionality to ease handling with
Sphinx RestructedText Files used e.g. in Online-Help for C++ Projects. The Plugin does not provide a builder for
Sphinx Projects, the Plugin integrates Sphinx with

* Editor support
* Build output parsing
* Preview of HTML Sphinx Builder output

The plugin is activated for Sphinx Restructured Text files with the exentions "*.rst"

A specific Editor for these Files is registered in QtCreator. To open other files, just open the files with the apropriate "Sphinx Editor".

The Plugin registgers output Parsers for Sphinx warnings and errors. These could be enabled in the Project configuration of your project
as "Post Build" action. Using the Sphinx Parser will raise Issue. Unfortunately not all errors and warnings give a file location, so
not all are linked to an source file.

Project specific previews could be configured to view content of Sphinx files after build together with the source file.

Editor
======

Highlighting
------------

Format Toolbar
--------------

Snippets
--------

Completion
----------

Link to HTML output
-------------------

Build Parser
============

Plugin Configuration
====================


