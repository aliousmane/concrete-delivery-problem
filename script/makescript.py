import os
import sys


class CreateScript():
    def __init__(self, alg, walltime, mem, class_instance,input_file, workDir, exec_line, runtime=300):
        self.alg = alg
        self.walltime = walltime
        self.mem = mem
        self.class_instance = class_instance
        self.inputFile = workDir + input_file + '.txt'
        self.OutputFile = workDir + alg + '/' + input_file + '.sh'
        self.workDir = workDir
        self.exec_line = exec_line
        self.runtime = runtime


    def create(self):
        self.method()

    def method(self):
        with open(self.OutputFile, "w") as output_file:
            output_file.write("#! /bin/bash \n")
            with open(self.inputFile, "r") as input_file:
                for line in input_file:
                    instance_name = line.rstrip('.rmc\n')
                    header = self.headers()
                    filename = "../script/" + self.alg + "/" + instance_name + ".sb"
                    result_file = "test_"+ instance_name[0]+".csv"
                    weight=[10,20,5]
                    with open(filename, "w") as script_file:
                        script_file.write(header)
                        script_file.write(
                            self.exec_line + " ../instances/" + self.class_instance + "/" + instance_name + ".rmc " 
                            + str(self.runtime) +" "+ f"test_{instance_name[0]}.csv " )
  
                        script_file.close()
                    output_file.write("sbatch " + instance_name + ".sb\n")
            output_file.close()


    def headers(self):

        texte = "#! /bin/bash -l \n"
        texte += "#SBATCH --mem=" + self.mem + "g\n"
        texte += "#SBATCH --time=" + self.walltime + "\n"
        texte += "#SBATCH --account=def-cotej\n"
        texte += "#SBATCH --cpus-per-task=1\n"
        texte += "#SBATCH --output=../../log/{}.%x.out\n".format(self.alg)
        texte += "#SBATCH --error=../../log/{}.%x.err\n".format(self.alg)
        texte += "#SBATCH --mail-user=ousmane.wattara92@gmail.com \n"
        texte += "#SBATCH --mail-type=FAIL \n\n"
        texte += "cd " + self.workDir + "\n\n"

        return texte

    def heading(self):
        pass


if __name__ == "__main__":
    cdpA = CreateScript('cdp', '1:30:00', '3', 'cdp','cdpA','/home/alious/works/CimentQuebec/script/', './code',1800)
    cdpB = CreateScript('cdp', '1:30:00', '3', 'cdp','cdpB','/home/alious/works/CimentQuebec/script/', './code',1800)
    cdpA.create()
    cdpB.create()