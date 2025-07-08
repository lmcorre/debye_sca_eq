import PyPDF2 
#print(f"PyPDF2 version: {PyPDF2.__version__}")
import numpy as np
#print(f"Numpy version: {np.__version__}")
import os
import re


def create_dict(atomic_number, values, input_dict):
    
    element_headers = ['a1', 'a2', 'a3', 'a4', 'a5','b1', 'b2', 'b3', 'b4', 'b5'] 
    input_dict[f"{atomic_number}"] = {}
    for i, element in enumerate(element_headers):
        input_dict[f"{atomic_number}"][element] = values[i]
    return input_dict

path_pdf = './Lobato_Dyck_2014.pdf'
pages_table = [6,7,8,9,10] # page in the pdf reader (1 to N pages)

try:
    os.path.isfile(path_pdf)
except:
    print(f"File not found in the path: {path_pdf}!")




atomic_dict = {}
with open(path_pdf, 'rb') as file:
    pdf = PyPDF2.PdfReader(file)
    #print(f'Number of pages: {len(pdf.pages)}')
    for page in pages_table:
        text_page = (pdf.pages[page - 1]).extract_text()
        table_elements = text_page.split('Z')
        for i in range(1, len(table_elements),1):
            #find atomic number
            atomic_number_text = re.findall('=.+/C27',table_elements[i])[0]
            atomic_number_text = re.findall('[0-9] ?[0-9]? ?[0-9]?', atomic_number_text)[0]
            atomic_number_text = re.findall('[0-9]', atomic_number_text)
            atomic_number = ""
            for alga in atomic_number_text:
                atomic_number += alga
            atomic_number = int(atomic_number)
            #print(f"Atomic Number: {atomic_number}")
            #find the table values
            #print(table_elements[i])
            values = re.findall('-?[0-9][.][0-9]+e[+-]?[0-9][0-9]', table_elements[i])[2:]
            #print(f"Number of Values: {len(values)}")
            atomic_dict = create_dict(atomic_number, values, atomic_dict)
#print(atomic_dict['1'])
with open('table_test.txt','w') as file:
    for i in range(1, len(atomic_dict) + 1, 1):

        file.write(f"{i}: " + str(atomic_dict[f'{i}']) + "\n")
        

    file.close()



