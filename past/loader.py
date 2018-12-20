# coding: utf-8

print("\"Loader\" module import successful")

# Imports
import argparse
import json

class argHandler:
    args = {}
    def __init__(self):
        # Arguments handling with argparse
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

        # TODO: refactor if trajectory data and requests included in same json file
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
            print("Data files path: ", self.args.inputFile)
        if self.args.inputDir != None:
            print("Data dir path: ", self.args.inputDir)
        if self.args.requestFile != None:
            print("Request files path: ", self.args.requestFile)
        if self.args.requestDir != None:
            print("Request dir path: ", self.args.requestDir)
        if self.args.img != None:
            print("Image path:", self.args.img)


class trajectoryFilesHandler:
    # Dictionary of key: "file path", value: javascript object
    filesDict = {}

    # implicit __init__()
    # TODO: Trajectory files path getter

    def addFile(self, pathList):
        # If pathList is a list and handle each file
        if type(pathList) == list:
            for path in pathList:
                self.handleFile(path)

        # Else it's a path
        elif type(pathList) == str:
            data = self.handleFile(pathList)

        elif pathList == None:
            print("No path provided")


    def handleFile(self, path):
        if(path not in self.filesDict):
            self.filesDict[path] = {}
        with open(path) as jsonIn:
            # Load raw javascript objects list
            rawJSON = json.loads(jsonIn.read())
            # For each object, put in the dictionary with key "id" and the object as value
            # TODO: flatten dictionnary to get rid of object "id" key because already dictionnary key
            for i in range(len(rawJSON)):
                id = rawJSON[i]["id"]
                # Getting rid of javascript object "id" key as it's already the python dictionary key
                rawJSON[i].pop('id')
                self.filesDict[path][id] = rawJSON[i]
