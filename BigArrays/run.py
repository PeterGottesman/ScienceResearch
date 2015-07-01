#!/usr/bin/python

#from pushover import Pushover
import argparse
from datetime import datetime
from subprocess import check_call, CalledProcessError
from sys import argv

def run(args):
    maxprocs = args.maxprocs
    verify = args.verify
    overload = args.overload_allowed
    message_size = args.message_size
    binding = args.binding
    plot = args.plot
    
    if verify: vstring = "--verify "
    else: vstring = ""

    if overload: binding += ":overload-allowed" 

    if plot: pstring = "--plot "
    else: pstring = ""

    memory = ['sm', 'vader']
    time = datetime.now().strftime("%Y-%m-%d.%H%M%S")

    try: 
        check_call("mpicc -g -o bigarrays bigarrays.c -std=c11", shell=True)
    except CalledProcessError as err:
        print("Error in Compiling with code" + str(err.returncode))
        exit(1)

    try:
        check_call("mkdir -p data/" + time, shell=True)
    except CalledProcessError as err:
        print("Error in creating data directory with error code" + str(err.returncode))
        exit(1)


    for nprocs in range(2, maxprocs+1):
        for mem in memory:
            base = "data/" + time + "/np="+ str(nprocs) + "_shmem=" + mem + "log"
            logfile = base + ".out"
            mpirun_logfile = base + ".mpirun"

            run = "mpirun -np  " + str(nprocs) + " --bind-to " + binding + " --mca btl " + mem + ",self bigarrays " + pstring + vstring + "--message_size " + str(message_size) + " > " + logfile
            
            print(run)
            f = open(mpirun_logfile, 'w')
            f.write(run + "\n")
            f.close()

            try:
                check_call(run, shell=True)
            except CalledProcessError as err:
                print("mpirun failed with code" + str(err.returncode))
                exit(1)


            if plot:
                f = open(logfile, 'r')
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
        try:
            check_call("gnuplot -e \"sm='data/"+time+"/sm-data.txt'\" -e \"vader='data/"+time+"/vader-data.txt'\" -e \"out='data/"+time+"/output-graph.pdf'\" plot.gnu", shell=True)
        except CalledProcessError as err:
            print("plot failed with error code " + str(err.returncode))
            exit(1)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("maxprocs", type=int, help="maximum number of processes to be run")
    parser.add_argument("--verify", action="store_true", help="verify the correctness of the data sent")
    parser.add_argument("--overload-allowed", action="store_true", help="allow overload of cores")
    parser.add_argument("--message_size", default=50000, type=int, help="specify the size of each message sent, defaults to 50000")
    parser.add_argument("--binding", help="set the bind type for mpi runtime. Should be none, core or socket")
    parser.add_argument("--plot", action="store_true", help="use gnuplot to plot the output data to pdf. Specifying this disables debug output")
    args = parser.parse_args()
    run(args)

if __name__ == "__main__":
    main()
