# coding: utf-8

print("\"Loader\" module imported")

# Imports
import argparse
import json

class argHandle:
    args = {}
    def __init__(self):
        # Gestion d'arguments
        parser = argparse.ArgumentParser()

        groupInput = parser.add_mutually_exclusive_group(required=True)
        groupInput.add_argument("-i",
                                "--inputFile",
                                action = "store",
                                nargs = "+",
                                help = "Chemin du ou des fichiers JSON des données de trajectoires")
        groupInput.add_argument("-I",
                                "--inputDir",
                                action = "store",
                                nargs = "+",
                                help = "Chemin du ou des dossiers contenant les fichiers JSON des données de trajectoires")

        # TODO: refactoriser les json pour inclure les requêtes dans un même fichier
        groupRequest = parser.add_mutually_exclusive_group()
        groupRequest.add_argument("-r",
                                  "--requestFile",
                                  action = "store",
                                  nargs = "+",
                                  help = "Chemin du ou des fichiers JSON des trajectoires requêtes")
        groupRequest.add_argument("-R",
                                  "--requestDir",
                                  action = "store",
                                  nargs = "+",
                                  help = "Chemin du ou des dossiers contenant les fichiers JSON des trajectoires requêtes")


        parser.add_argument("--img",
                            required = False,
                            action = "store",
                            nargs = "+",
                            help = "Chemin de l'image sur laquelle on visualise les trajectoires")
        # TODO: add version ?

        self.args = parser.parse_args()

        if self.args.inputFile != None:
            print("Input files path: ", self.args.inputFile)
        if self.args.inputDir != None:
            print("Input dir path: ", self.args.inputDir)
        if self.args.requestFile != None:
            print("Input files path: ", self.args.requestFile)
        if self.args.requestDir != None:
            print("Input dir path: ", self.args.requestDir)
        if self.args.img != None:
            print("Image path:", self.args.img)

class trajectoryList:
    data = []
    def __init__(self, pathList):
        # Si pathList est une liste
        if type(pathList) == list:
            for path in pathList:
                # TODO: gérer plusieurs listes
                with open(path) as jsonIn:
                    self.data = json.loads(jsonIn.read())

        # Sinon c'est un chemin
        with open(path) as jsonIn:
            self.data = json.loads(jsonIn.read())
