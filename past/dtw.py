# coding: utf-8

print("\"DTW\" module loaded")

#import json
import numpy as np
import scipy.spatial

#TODO: wrapper pour les comparaisons (comparaison.DTW, comparaison.sDTW, comparaison.HAUSDORF)
class dtw:
    scoreList = []
#    trajectories = []

    def __init__(self, trajectoryList, request):
        # TODO: passer les trajectoires en membre de la classe et passer les indices dans les fonctions
        #self.trajectories = trajectoryList
        for i in range(len(trajectoryList)):
            self.scoreList.append([i, self.dtw(trajectoryList[request], trajectoryList[i])[-1, -1]])

        self.afficherScore()


    def dtw(self, request, target, log=False):
        # Taille des trajectoires
        n, m = len(request["track"]), len(target["track"])
        if log:
            print("Trajectoire requête # {idRequete!s} de taille: {tailleRequete}; Trajectoire cible #{idCible!s} de taille: {tailleCible}".format(idRequete = request["id"], tailleRequete = str(n), idCible = target["id"], tailleCible = str(m)))

        # Lambda pour calculer les distances
        dist = lambda a, b: scipy.spatial.distance.pdist(np.array([[request["track"][a]["x"],
                                                                    request["track"][a]["y"]],
                                                                   [ target["track"][b]["x"],
                                                                     target["track"][b]["y"]]]))

        result = np.zeros([n, m])

        # Première valeur
        result[0, 0] = dist(0, 0)

        # Première colonne
        for i in range(1, n):
            result[i, 0] = dist(i, 0) + result[i-1, 0]

        # Première ligne
        for j in range(1, m):
            result[0, j] = dist(0, j) + result[0, j-1]

        # Reste de la matrice
        for i in range(1, n):
            for j in range(1, m):
                distance = dist(i, j)
                result[i, j] = min((distance + result[i-1, j], distance + result[i-1, j-1], distance + result[i, j-1]))

        return result

    def afficherScore(self):
        # Tri
        resultats = sorted(self.scoreList, key = lambda d: d[1])
        # Parcours
        for traj in resultats:
            print("#{id:2d}: score = {score:7.1f}".format(id = traj[0], score=traj[1]))

#    def exportMatrix(self):
#        np.savetxt("foo.csv", dtw(trajectories[0], trajectories[1]), delimiter=",")
