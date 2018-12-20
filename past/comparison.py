# coding: utf-8

print("\"Comparison\" module import successful")

import numpy as np
import scipy.spatial

#TODO: wrapper comparisons (comparison.DTW, comparison.sDTW, comparison.HAUSDORF)
class comparison:
    # Results dictionary (key: matched request, value: dictionary(key: matched target, value: score))
    scoreDict = {}
    # Trajectories dictionaries (key: trajectoryId, value: trajectories dictionnary(keys: "track", maybe "time"))
    trajectoryDict = {}
    requestDict = {}

    def __init__(self, trajectoryDict, requestDict = None):
        self.trajectoryDict = trajectoryDict
        if(requestDict == None):
            print("No request dictionary provided")
        else:
            self.requestDict = requestDict

    def compare(self, index, fromDataSet = False):
        # TODO: dictionary "index" key to (datasetType, datasetId, trackID) tuple

        if(index not in self.scoreDict):
            self.scoreDict[index] = {}

        # Run the comparison
        for id in list(self.trajectoryDict.keys()):
            self.scoreDict[index][id] = self.dtw(index, id, fromDataSet)[-1, -1]

    def dtw(self, requestId, targetId, fromDataSet = False, log = False):
        # Set trajectories dicts to use
        if(fromDataSet):
            request = self.trajectoryDict[requestId]
        else:
            request = self.requestDict[requestId]
        target = self.trajectoryDict[targetId]

        # Trajectories length
        n, m = len(request["track"]), len(target["track"])
        if log:
            print("Trajectoire requête # {requestId!s} de taille: {requestSize}; Trajectoire cible #{targetId!s} de taille: {targetSize}".format(requestId = requestId, requestSize = str(n), targetId = targetId, targetSize = str(m)))

        # Lambda to compute distances
        dist = lambda a, b: scipy.spatial.distance.pdist(np.array([[request["track"][a]["x"],
                                                                    request["track"][a]["y"]],
                                                                   [ target["track"][b]["x"],
                                                                     target["track"][b]["y"]]]))

        result = np.zeros([n, m])

        # First value
        result[0, 0] = dist(0, 0)

        # First column
        for i in range(1, n):
            result[i, 0] = dist(i, 0) + result[i-1, 0]

        # First row
        for j in range(1, m):
            result[0, j] = dist(0, j) + result[0, j-1]

        # Rest of the matrix
        for i in range(1, n):
            for j in range(1, m):
                distance = dist(i, j)
                result[i, j] = min((distance + result[i-1, j], distance + result[i-1, j-1], distance + result[i, j-1]))

        return result

    def printScore(self, index):
        # Sort
        resultats = [(k, self.scoreDict[index][k]) for k in sorted(self.scoreDict[index], key = self.scoreDict[index].get)]

        # Print
        for traj in resultats:
            print("#{id:2d}: score = {score:7.1f}".format(id = traj[0], score=traj[1]))

# REFACTOR THIS
#    def exportMatrix(self):
#        np.savetxt("foo.csv", dtw(trajectories[0], trajectories[1]), delimiter=",")
