#!/usr/bin/python3
# coding: utf-8

import past.loader
import past.dtw

# Gestion des arguments
argHandle = past.loader.argHandle()
trajectoryList = past.loader.trajectoryList(argHandle.args.inputFile)
past.dtw.dtw(trajectoryList.data, 0)
