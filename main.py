#!/usr/bin/python3
# coding: utf-8

import past.loader
import past.comparison

# Arguments handler
argHandle = past.loader.argHandler()

# New files handler
filesHandler = past.loader.trajectoryFilesHandler()
filesHandler.addFile(argHandle.args.inputFile, past.loader.EFileType.DATA)
filesHandler.addFile(argHandle.args.requestFile, past.loader.EFileType.REQUEST)

# Make comparison
files = list(filesHandler.filesDict.keys())
comparison = past.comparison.comparison(filesHandler.filesDict[files[0]], filesHandler.filesDict[files[1]])

# Example where the first trajectory of file "tracksWithTime.json" is matched with all trajectories of the same file
print("Matching first trajectory from 'tracksWithTime.json' to all trajectoryies of 'tracksWithTime.json'")
comparison.compare(0, True)
comparison.printScore(0)

# Example where the first request of file "queriesFull.json" is matched with all trajectories of file "tracksWithTime.json"
print("\nMatching first request from 'queriesFull.json' to all trajectoryies of 'tracksWithTime.json'")
comparison.compare(0)
comparison.printScore(0)
