#!/usr/bin/python

import os, sys, getopt

# funtion that cuts of the file from the whole path
def get_dir(path):
    tokens = path.split("/")        # split string into tokens
    tokens = tokens[:-1]            # cur the last token which is the file

    dir_str = ""
    for i in range(len(tokens)):    # iterate through every token and...
        dir_str += tokens[i] + "/"  # put them together
    return dir_str                  # result: path with out the file at the end

def main(argv):
    # syntax: makedirectory.py [<path>] ... anything else is invalid and will print this error message
    if len(argv) < 1:
        print("Invalid argument length: " + str(len(argv)) + ", requiered 1.")
        print("Syntax: makedirectory.py [<path>]")
        sys.exit(-1)
    
    # if the input was correct...
    for i in range(len(argv)):                          # iterate through the directories...
        os.makedirs(get_dir(argv[i]), exist_ok = True)  # and create them, dont care if they already exsits
    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])
