#!/usr/bin/python3
# coding: utf-8

import past.loader
import past.comparison

# Arguments handler
argHandle = past.loader.argHandler()

# New files handler
filesHandler = past.loader.trajectoryFilesHandler()
filesHandler.addFile(argHandle.args.inputFile)
filesHandler.addFile(argHandle.args.requestFile)

# Make comparison
files = list(filesHandler.filesDict.keys())
comparison = past.comparison.comparison(filesHandler.filesDict[files[0]])
comparison.compare(0, True)
comparison.printScore(0)
