import os
import sys
import importlib

Binder_SIF = None

def load_library(model_path):
    global Binder_SIF
    sys.path.append(model_path)
    if Binder_SIF is None:
        Binder_SIF = importlib.import_module("Binder_SIF")

def autoload_library():
    model_path = os.environ.get("SIF_MODULE_PATH")    
    if model_path is None:
        raise ValueError("The environment variable 'SIF_MODULE_PATH' is not set")
    load_library(model_path)

autoload_library()

class Model(Binder_SIF.RawModel):
    def __init__(self, fpath):
        super().__init__(fpath)
