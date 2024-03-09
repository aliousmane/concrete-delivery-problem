#%%
import os
import pandas as pd

#%%
data = [
    {"Name": "20201121", "Instance": "C_13_11_12_1", "folder": "Small"},
    {"Name": "20201010", "Instance": "C_13_5_8_1", "folder": "Small"},
    {"Name": "20201107", "Instance": "C_18_6_11_2", "folder": "Small"},
    {"Name": "20201031", "Instance": "C_15_4_7_2", "folder": "Small"},
    {"Name": "20201114", "Instance": "C_19_7_8_2", "folder": "Small"},
    {"Name": "20201003", "Instance": "C_29_10_14_3", "folder": "Small"},
    {"Name": "20201128", "Instance": "C_31_8_11_3", "folder": "Small"},
    {"Name": "20201024", "Instance": "C_35_9_10_3", "folder": "Small"},
    {"Name": "20201021", "Instance": "C_76_40_43_6", "folder": "Medium"},
    {"Name": "20201013", "Instance": "C_104_42_47_8", "folder": "Medium"},
    {"Name": "20201109", "Instance": "C_94_63_70_7", "folder": "Medium"},
    {"Name": "20201026", "Instance": "C_116_67_78_8", "folder": "Medium"},
    {"Name": "20201016", "Instance": "C_116_71_82_8", "folder": "Medium"},
    {"Name": "20201130", "Instance": "C_117_57_70_6", "folder": "Medium"},
    {"Name": "20201125", "Instance": "C_137_79_83_7", "folder": "Medium"},
    {"Name": "20201007", "Instance": "C_127_66_80_8", "folder": "Medium"},
    {"Name": "20201123", "Instance": "C_128_68_74_7", "folder": "Medium"},
    {"Name": "20201020", "Instance": "C_136_85_97_8", "folder": "Medium"},
    {"Name": "20201124", "Instance": "C_128_78_85_8", "folder": "Medium"},
    {"Name": "20201002", "Instance": "C_131_77_85_8", "folder": "Medium"},
    {"Name": "20201116", "Instance": "C_137_89_97_7", "folder": "Medium"},
    {"Name": "20201126", "Instance": "C_133_84_98_7", "folder": "Medium"},
    {"Name": "20201102", "Instance": "C_132_98_109_8", "folder": "Large"},
    {"Name": "20201009", "Instance": "C_129_101_119_8", "folder": "Large"},
    {"Name": "20201106", "Instance": "C_141_114_136_8", "folder": "Large"},
    {"Name": "20201120", "Instance": "C_140_114_132_8", "folder": "Large"},
    {"Name": "20201111", "Instance": "C_143_101_123_8", "folder": "Large"},
    {"Name": "20201014", "Instance": "C_137_112_129_8", "folder": "Large"},
    {"Name": "20201110", "Instance": "C_142_114_129_8", "folder": "Large"},
    {"Name": "20201030", "Instance": "C_149_98_122_8", "folder": "Large"},
    {"Name": "20201118", "Instance": "C_139_98_108_8", "folder": "Large"},
    {"Name": "20201113", "Instance": "C_144_108_122_8", "folder": "Large"},
    {"Name": "20201112", "Instance": "C_142_92_107_8", "folder": "Large"},
    {"Name": "20201006", "Instance": "C_138_114_136_8", "folder": "Large"},
    {"Name": "20201029", "Instance": "C_150_127_136_8", "folder": "Large"},
    {"Name": "20201104", "Instance": "C_148_112_131_8", "folder": "Large"},
]

df = pd.DataFrame(data)
#%%
def create_instance(input, output):
    with open(f'../cq/{input}.txt', 'r') as f:
        lines = f.readlines()
        # Extract the number of drivers from the first line
        num_drivers = int(lines[0].split()[-1])

        # Extract and format the desired columns from lines 2 to x+1 (where x is the number of drivers)
        extracted_lines = []
        extracted_lines.append(lines[0].upper())
        extracted_lines.append("ID PLANT_ID CAPACITY SHIFT_START\n")
        for line in lines[1:num_drivers+1]:
            parts = line.split()
            extracted_line = ' '.join([parts[0], parts[-3],parts[-2], parts[-1]]) + '\n'
            extracted_lines.append(extracted_line)

    # Extract the number of clients and the number of orders
    num_clients = int(lines[num_drivers+1].split()[1])
    
    # Extract and format the desired columns for clients
    client_start = num_drivers+2
    extracted_lines.append(lines[client_start-1].upper())
    extracted_lines.append("ID MATRIX_ID DUE_DATE DEMAND #ORDER\n")
    client_end = client_start + num_clients
    for line in lines[client_start:client_end]:
        parts = line.split()
        extracted_line = ' '.join([parts[0], parts[3], parts[5], parts[6],parts[7]]) + '\n'
        extracted_lines.append(extracted_line)

    # Copy the lines for orders
    num_orders = int(lines[client_end].split()[1])
    order_start = client_end+1
    order_end = order_start + num_orders
    extracted_lines.append(lines[order_start-1].upper())
    extracted_lines.append("ID CLIENT_ID DEMAND PLANT_ID\n")
    for line in lines[order_start:order_end]:
        extracted_lines.append(line)

    # Extract the number of plants
    num_plants = int(lines[order_end].split()[1])

    # Extract and format the desired columns for plants
    extracted_lines.append(lines[order_end].upper())
    extracted_lines.append("ID MATRIX_ID CAPACITY\n")
    plant_start = order_end + 1
    plant_end = plant_start + num_plants
    for line in lines[plant_start:plant_end]:
        parts = line.split()
        extracted_line = ' '.join([parts[0], parts[1], parts[3]]) + '\n'
        extracted_lines.append(extracted_line)

# # Save the extracted lines to a new file
    with open(output_path, 'w') as f:
        f.writelines(extracted_lines)
        
# %%
for i,name in enumerate(df['Name']):
    print(name)
    output_path = f"{df.loc[i]['folder'].lower()}/{df.loc[i]['Instance']}.rmc"
    create_instance(name,output_path)
    print(output_path)
    # break
# %%
    