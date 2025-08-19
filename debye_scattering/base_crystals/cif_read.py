import os
import re
def read_from_path(cif_path, input_dict):
    if(os.path.isfile(cif_path) == False):     
        raise("No file in" + cif_path)
    input_dict["unit"] = "nm"
    with open(cif_path) as file:
        for line in file:
            if("cell_formula_units_Z" in  line):
                    input_dict["N_atoms_unit_cell"]  = float(re.findall("[0-9]", line)[0])
            if("cell_length_a" in line):
                input_dict["length_a"] =  float(re.findall("[0-9]+(?:[.][0-9]+)?", line)[0])/10   
            if("cell_length_b" in line):
                input_dict["length_b"] =  float(re.findall("[0-9]+(?:[.][0-9]+)?", line)[0])/10    
            if("cell_length_c" in line):
                input_dict["length_c"] =  float(re.findall("[0-9]+(?:[.][0-9]+)?",line)[0])/10
            if("cell_volume" in line):
                input_dict["cell_volume"] = float(re.findall("[0-9]+(?:[.][0-9]+)?",line)[0])/10**3 



    
