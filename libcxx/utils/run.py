#!/usr/bin/env python
#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

"""run.py is a utility for running a program.

It can perform code signing, forward arguments to the program, and return the
program's error code.
"""

import argparse
import os
import platform
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--execdir', type=str, required=True)
    parser.add_argument('--codesign_identity', type=str, required=False, default=None)
    parser.add_argument('--env', type=str, nargs='*', required=False, default=dict())
    parser.add_argument('--test-executables', type=str, nargs=argparse.ZERO_OR_MORE, default=None)
    parser.add_argument("command", nargs=argparse.ONE_OR_MORE)
    args = parser.parse_args()
    commandLine = args.command

    test_executables = args.test_executables if not args.test_executables else [commandLine[0]]
    # Do any necessary codesigning.
    if args.codesign_identity:
        for exe in test_executables:
            rc = subprocess.call(['xcrun', 'codesign', '-f', '-s', args.codesign_identity, exe], env={})
            if rc != 0:
                sys.stderr.write('Failed to codesign: ' + exe)
                return rc

    # Extract environment variables into a dictionary
    env = {k : v  for (k, v) in map(lambda s: s.split('=', 1), args.env)}
    if platform.system() == 'Windows':
        # Pass some extra variables through on Windows:
        # COMSPEC is needed for running subprocesses via std::system().
        if 'COMSPEC' in os.environ:
            env['COMSPEC'] = os.environ.get('COMSPEC')
        # TEMP is needed for placing temp files in a sensible directory.
        if 'TEMP' in os.environ:
            env['TEMP'] = os.environ.get('TEMP')

    # Run the command line with the given environment in the execution directory.
    return subprocess.call(commandLine, cwd=args.execdir, env=env, shell=False)


if __name__ == '__main__':
    exit(main())
