#!/usr/bin/env python3

import argparse
import os
import subprocess

def get_arguments():
    parser = argparse.ArgumentParser(description='Build Qt Creator for packaging')
    parser.add_argument('--qt-path', help='Path to Qt', required=True)
    parser.add_argument('--qtc-path',
                        help='Path to Qt Creator installation including development package',
                        required=True)
    parser.add_argument('--name', help='Name to use for build results', default="qtcreator-sphinx-plugin")
    parser.add_argument('--src', help='Path to sources', default=".")
    parser.add_argument('--build', help='Path that should be used for building', default="build")
    parser.add_argument('--output-path', help='Output path for resulting 7zip files', default="build")
    parser.add_argument('--with-docs', help='Build and install documentation.',
                        action='store_true', default=False)
    parser.add_argument('--build-type', help='Build type to pass to CMake (defaults to RelWithDebInfo)',
                        default='RelWithDebInfo')
    args = parser.parse_args()
    return args

def check_print_call(command, workdir=None, env=None):
    print('------------------------------------------')
    print('COMMAND:')
    print(' '.join(['"' + c.replace('"', '\\"') + '"' for c in command]))
    print('PWD:      "' + (workdir if workdir else os.getcwd()) + '"')
    print('------------------------------------------')
    subprocess.check_call(command, cwd=workdir, env=env)

def build(args):
    check_print_call(["python",f"{args.qtc_path}/share/qtcreator/scripts/build_plugin.py", "--name", args.name, "--src", args.src, "--build", args.build, "--qt-path", args.qt_path, "--qtc-path", args.qtc_path, "--output-path", args.output_path])

def main():
    args = get_arguments()
    build(args)

if __name__ == '__main__':
    main()


