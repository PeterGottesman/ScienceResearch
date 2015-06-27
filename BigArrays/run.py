#!/usr/bin/python

#from pushover import Pushover
import argparse
from datetime import datetime
from subprocess import call
from sys import argv

def run(args):
    maxprocs = args.maxprocs
    verify = args.verify
    message_size = args.message_size
    binding = args.binding
    plot = args.plot
    
    if verify: vstring = "--verify "
    else: vstring = ""

    if binding == 0: bind = "none"
    elif binding == 1: bind = "core"
    elif binding == 2: bind = "socket"

    if plot: pstring = "--plot "
    else: pstring = ""

    memory = ['sm', 'vader']
    time = datetime.now().strftime("%Y-%m-%d.%H%M%S")

    call("mpicc -g -o bigarrays bigarrays.c -std=c11", shell=True)
    call("mkdir -p data/" + time, shell=True)

    for nprocs in range(2, maxprocs+1):
        for mem in memory:
            filename = "data/" + time + "/np="+ str(nprocs) + "_shmem=" + mem + "log.out" 
            call("mpirun -np  " + str(nprocs) + " --bind-to " + bind + " --mca btl " + mem + ",self bigarrays " + pstring + vstring + "--message_size " + str(message_size) + " > " + filename, shell=True)

            if plot:
                f = open(filename, 'r')
                data = []
                for line in f:
                    data += line.split()
                data = [float(i) for i in data]
                f.close()
                
                data = data[1::2]
                np = len(data)
                avg = sum(data)/len(data)

                f = open("data/" + time + "/" + mem + "-data.txt", 'a')
                f.write(str(np) + " " + str(avg) + "\n")
                f.close()

    if plot:
        call("gnuplot -e \"sm='data/"+time+"/sm-data.txt'\" -e \"vader='data/"+time+"/vader-data.txt'\" -e \"out='data/"+time+"/output-graph.pdf'\" plot.gnu", shell=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("maxprocs", type=int, help="maximum number of processes to be run")
    parser.add_argument("--verify", action="store_true", help="verify the correctness of the data sent")
    parser.add_argument("--message_size", default=50000, type=int, help="specify the size of each message sent, defaults to 50000")
    parser.add_argument("--binding", type=int, default=0, help="set the bind type for mpi runtime. 0=bind-to-none(default) 1=bind-to-core 2=bind-to-socket")
    parser.add_argument("--plot", action="store_true", help="use gnuplot to plot the output data to pdf. Specifying this disables debug output")
    args = parser.parse_args()
    run(args)

if __name__ == "__main__":
    main()
