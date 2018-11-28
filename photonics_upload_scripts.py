# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""
import pandas as pd
import tkinter as tk
import tkinter.filedialog as filedialog
import numpy as np
import sqlalchemy
from sqlalchemy.orm import sessionmaker
#import mysql.connector
import os
import fnmatch

def get_mask_file(default_dir, prompt):
    root = tk.Tk()
    root.withdraw()
    filename = filedialog.askopenfilename(initialdir = default_dir, title = prompt)
    return filename

def get_directory_user_prompt(default_dir, prompt):
    root = tk.Tk()
    root.withdraw()
    directory = filedialog.askdirectory(initialdir = default_dir, title = prompt)
    return directory

def retrieve_detector_files_from_directory(directory):
    print("Parsing directory: ", directory)
    filepaths = []
    
    for item in os.listdir(directory):
        if "*Flash*.csv" in item:
            print(item)
        
    
    for root, dirs, files in os.walk(directory):
        for filename in fnmatch.filter(files, '*Flash*.csv'):
            try:
                filepath = os.path.join(root, filename)
                filepaths.append(filepath)

            except:
                print("Error parsing file path. Sorry!")
    return filepaths

def retrieve_two_point_files_from_directory(directory):
	print("Parsing directory: ", directory)
	filepaths = []
	for root, dirs, files in os.walk(directory):
		for filename in fnmatch.filter(files, '*.csv'):
			if "Flash" not in filename and "PH001" not in filename:
				try:
					filepath = os.path.join(root, filename)
					filepaths.append(filepath)
				except:
					print("Error parsing file path. Sorry!")
	return filepaths

class detector_object():
    def __init__(self,device_name,x_coord, y_coord,mesa_x,mesa_r,mesa_area,active_x,active_r,active_area, maskname,part_type):
        self.Name = device_name
        self.x_coord = x_coord
        self.y_coord = y_coord
        self.mesa_x = mesa_x
        self.mesa_r = mesa_r
        self.mesa_area = mesa_area
        self.active_x = active_x
        self.active_r = active_r
        self.active_area = active_area
        self.maskname = maskname
        self.part_type = part_type
#
def define_detector_objects(filename):
    mask_df = pd.read_csv(filename)
    detector_objects = []
    try:
        for index,row in mask_df.iterrows():
            maskname = row['Process Name']
            x_coord = row['X Loc']
            y_coord = row['Y Loc']
            mesa_x = row['Mesa X']
            mesa_r = row['Mesa R']
            mesa_area = row['Mesa Area']
            active_x = row['Active X']
            active_r = row['Active R']
            active_area = row['Active Area']
            device_name = row['Device Name']
            part_type = 'Detector'
            current_object = detector_object(device_name,x_coord, y_coord,mesa_x,mesa_r,mesa_area,active_x,active_r,active_area, maskname,part_type)
            detector_objects= np.append(detector_objects, current_object)
    except:
        print('Error parsing file')
    return detector_objects

def parse_filepath(filepath):
    filename = os.path.basename(filepath)
    parts = filename.split("_")
    wafer_name = parts[0]
    return wafer_name

def create_list_to_upload(filepaths, connection):
    files_to_upload = []
        
    for filepath in filepaths:
        try:
            filename = os.path.basename(filepath)
            print("Checking to see if file", filename, "was previously uploaded...")
            query = f"SELECT filename FROM uploaded_filenames WHERE filename = '{filename}'"
            check = connection.execute(query)

            # check to see if the filename exists in the database table already 
            if check.first():
                print("File", filename, "previously uploaded. Skipping.")
                pass # do nothing
            else:
                print("File", filename, "not previously uploaded. Queueing.")
                files_to_upload.append(filepath) # append full filepath to list of files to upload
        
        except:
            print('There was an error checking to see if the file was previously uploaded.')
            raise KeyboardInterrupt

    return files_to_upload
def create_detector_liv_dataframes(pho,epi,filepath,wafername):
    params_df = pd.read_csv(filepath,index_col=0,usecols=[0,1],header=None).T
    params_df.rename(
        columns = {"Isotype Scalar": "input_current_ma", 
                "P_In (W)": "incident_power_mw",
                   "Corrected Irradiance (W/cm_sq)":"incident_power_density_mw",
                   "Chuck Temperature (degrees C)":"chuck_temperature",
                   "Ambient Temperature (degrees C)": "ambient_temperature",
                   "Session Date":"session_date",
                   "Date":"measurement_date",
                   "Comment": "comments",
                   "Measurement Recipe ID": "detector_measurement_recipe_id"
                  }, 
        inplace = True)
    xloc = int(float(params_df['X Coord'][1]))
    yloc = int(float(params_df['Y Coord'][1]))
    device_id = get_detector_device_id(pho,epi, wafername, int(float(params_df['X Coord'][1])),int(float(params_df['Y Coord'][1])))
    try:    
        params_df['session_date'] = pd.to_datetime(params_df['session_date'])
        params_df['measurement_date'] = pd.to_datetime(params_df['measurement_date'])
    except:
        x=1
    param_columns = ["input_current_ma","incident_power_mw","incident_power_density_mw","chuck_temperature","ambient_temperature","session_date","measurement_date","comments","detector_measurement_recipe_id"]
    params_df_new = params_df[param_columns].copy()
    data_df = pd.read_csv(filepath,index_col=0,skiprows=44,header=None).T
    params_df_new['device_id'] = device_id 
    data_df.rename(
        columns = {"V": "voltage_v", 
                "I": "current_ma"}, inplace=True)
    
    data_columns = ["voltage_v","current_ma"]
    data_df_new = data_df[data_columns].copy()

    return params_df_new,data_df_new
def create_two_point_dataframe(filepath, wafername, pho, epi):
    df = pd.read_csv(filepath)

    df.columns = df.columns.str.lower().str.replace(' ', '_')
    df.rename(
        columns = {"time": "measurement_date", 
                   "imeas": "i_meas", 
                   "vmeas": "v_meas", 
                   "vset": "v_set", 
                   "icomp": "i_comp", 
                   "jset": "j_set", 
                   "iset": "i_set", 
                   "vcomp": "v_comp", 
                   "testid": "raw_test_id",
                   "sessiontime": "session_date"}, 
        inplace = True)
    
    df['measurement_date'] = pd.to_datetime(df['measurement_date'])
    df['session_date'] = pd.to_datetime(df['session_date'])
    
    device_ids = []
    for index, row in df.iterrows():
        x = row['x']
        y = row['y']
        device_id = get_detector_device_id(pho,epi, wafername, int(float(x)),int(float(y)))
        device_ids = np.append(device_ids,int(device_id))
    df['device_id'] = device_ids
    columns = ['raw_test_id', 'device_id', 'i_meas', 'v_meas', 'i_comp', 'v_comp', 'j_set', 'i_set', 'v_set', 'laser_power', 'session_date','measurement_date', 'operator', 'selected']
    df_new = df[columns].copy()
    return df_new
def prepare_dataframe(connection, filename):
    mask_df = pd.read_csv(filename)
    maskname=os.path.basename(filename).split('_')[0]
    mask_df.rename(
        columns = {"Process Name": "mask_name", 
                   "Device Name": "detector_part_name", 
                   "X Loc": "x_loc", 
                   "Y Loc": "y_loc", 
                   "Mesa R": "mesa_r", 
                   "Mesa X": "mesa_x", 
                   "Mesa Area": "mesa_area",
                   "Active R": "active_r", 
                   "Active X": "active_x", 
                   "Active Area": "active_area"}, 
        inplace = True)
    columns = ['detector_part_name', 'x_loc', 'y_loc', 'mesa_x', 'mesa_r', 'mesa_area', 'active_x', 'active_r', 'active_area', 'mask_name']
    new_mask_df = mask_df[columns].copy()
    new_mask_df['mask_id'] = get_mask_id(connection, maskname)
    new_mask_df['part_type'] = 'Detector'
    return new_mask_df

def define_connection(user, server, schema,password=None):
    if password != None:    
        engine = sqlalchemy.create_engine('mysql+mysqlconnector://' + user + ':'+ password + '@' + server + '/' + schema, echo = False)
    else:
        engine = sqlalchemy.create_engine('mysql+mysqlconnector://' + user  + '@' + server + '/' + schema, echo = False)
    connection = engine.connect()
    session = sessionmaker(bind=engine)
    return connection, engine

def close_connection(connection, engine):
    connection.close()
    engine.dispose()

def define_epi_connection(epi_user,epi_server, epi_schema, epi_password):
    engine = sqlalchemy.create_engine('mysql+mysqlconnector://' + epi_user + ':'+ epi_password + '@' + epi_server + '/' + epi_schema, echo = False)
    connection = engine.connect()
    session = sessionmaker(bind=engine)
    return connection

def get_mask_id(connection, mask_name):
    query = f"SELECT device_mask_id FROM epi_device_mask WHERE device_mask = '{mask_name}'"
    
    sql_object = connection.execute(query)
    
    for row in sql_object:
        device_mask_id = row[0]
    
    return device_mask_id

def get_detector_device_id(pho,epi,wafername, x_loc, y_loc):

    wafer_id = get_wafer_id(epi, wafername)
    query = f"SELECT device_mask_id FROM epi_wafer WHERE wafer_id = '{wafer_id}'"
    sql_object = epi.execute(query)
    for row in sql_object:
        mask_id = row[0]
    query = f"SELECT detector_part_id FROM detector_mask_info WHERE mask_id = '{mask_id}' AND x_loc = '{x_loc}' AND y_loc = '{y_loc}'"
    
    sql_object = pho.execute(query)
    
    for row in sql_object:
        part_id= row[0]
    
    
    query = f"SELECT device_id FROM relations_table WHERE wafer_id = '{wafer_id}' AND part_id = '{part_id}'"
    
    sql_object = pho.execute(query)
    
    for row in sql_object:
        device_id = row[0]
    
    return device_id

def get_detector_liv_measurement_id(connection):
    query = f"SELECT max(detector_sweep_measurement_id) FROM detector_measurement_parameters"
    sql_object = connection.execute(query)
    for row in sql_object:
        measurement_id = row[0]
    return measurement_id

def get_wafer_id(connection, wafer_name):
    query = f"SELECT wafer_id FROM epi_wafer WHERE wafer_name = '{wafer_name}'"
    
    sql_object = connection.execute(query)
    for row in sql_object:
        wafer_id = row[0]
    return wafer_id

def upload_df(connection,table,df):
    df.to_sql(name=table,con=connection,if_exists = 'append', index = False)      

def append_filename(filename):
    column = ['filename']
    df=pd.DataFrame(index=[0],columns=column)
    df['filename'] = filename
    return df

def discover_detector_liv_measurement_recipe(meas_id,connection):
    query = "select * from detector_measurement_parameters INNER JOIN (select * from detector_measurement_liv_data where detector_sweep_measurement_id=" +str(meas_id) + ") as t on detector_measurement_parameters.detector_sweep_measurement_id=" + str(meas_id) 
    measurement_df = pd.read_sql_query(query,connection)
    start_voltage_v = min(measurement_df['voltage_v'])
    end_voltage_v = max(measurement_df['voltage_v'])
    number_data_points = measurement_df.shape[0]
    comments = "created retroactively"
    return (start_voltage_v,end_voltage_v,number_data_points,comments)

def check_and_create_meas_recipe(meas_id, username, target_ip, target_db, password=None):
    query = "select detector_measurement_recipe_id from detector_measurement_parameters where detector_sweep_measurement_id="+str(meas_id)
    pho, engine = define_connection(username, target_ip, target_db, password=None)
    sql_object = pho.execute(query)
    recipe_id=None
    for row in sql_object:
        recipe_id = row[0]

    if recipe_id != None:
        print('This has a recipe already')
        close_connection(pho, engine)
        pass
  
    else:
        start_voltage_v,end_voltage_v,number_data_points,comments = discover_detector_liv_measurement_recipe(meas_id, pho)
        recipe_test = check_if_recipe_exists(pho,start_voltage_v,end_voltage_v,number_data_points)
        if len(recipe_test) == 0:
            new_recipe = pd.DataFrame({'detector_measurement_recipe_id':'NULL',
                                       'start_voltage_v':start_voltage_v,
                                       'end_voltage_v':end_voltage_v,
                                       'number_data_points':number_data_points,
                                       'I_compliance_ma':'NULL',
                                       'sweep_delay_s':'NULL',
                                       'direction':'',
                                       'comments':comments,
                                       'common_recipe':0,
                                       'setup_id':'NULL'}, index=[0])

            upload_df(pho, 'detector_measurement_recipe', new_recipe)
            query = "select max(detector_measurement_recipe_id) from detector_measurement_recipe"
            recipe_id = pd.read_sql_query(query,pho)['max(detector_measurement_recipe_id)'][0]
            query = "UPDATE detector_measurement_parameters SET detector_measurement_recipe_id=" + str(recipe_id) +" WHERE detector_sweep_measurement_id="+str(meas_id)
            pho.execute(query)
            close_connection(pho, engine)
            print("New recipe successfully uploaded")
        else:
            query = "UPDATE detector_measurement_parameters SET detector_measurement_recipe_id=" + str(recipe_test[0]) +" WHERE detector_sweep_measurement_id="+str(meas_id)
            pho.execute(query)
            print("Recipe found and updated")
            close_connection(pho, engine)

def check_if_recipe_exists(connection,start_voltage_v,end_voltage_v,number_data_points):
    query = "SELECT * FROM detector_measurement_recipe WHERE (start_voltage_v < " + str(start_voltage_v +.01)+ " AND start_voltage_v > "+ str(start_voltage_v -.01)+ ") AND (end_voltage_v < " + str(end_voltage_v +.01)+ " AND end_voltage_v > "+ str(end_voltage_v -.01)+ ") AND number_data_points= " + str(number_data_points) + " AND comments = 'created retroactively'"
    recipe_id = pd.read_sql_query(query,connection)['detector_measurement_recipe_id']
    return recipe_id


def upload_detector_mask_file(username, target_ip, target_db, password,epi_username, epi_server, epi_db, epi_password):
    filename = get_mask_file('C:', 'Select a detector mask file:')
    filename_end = os.path.basename(filename)
    print('Parsing' + filename_end)
    pho_connection, pho_engine = define_connection( username, target_ip, target_db, password)
    epi_connection, epi_ingine = define_connection(epi_username, epi_server, epi_db, epi_password)

    query = f"SELECT * FROM uploaded_filenames WHERE filename='{filename_end}'"
    sql_object = pho_connection.execute(query)
    filetest = 'none'
    for row in sql_object:
        filetest = row[0]
    if filetest != 'none':
        print('Mask file previously uploaded')
        return 0
    df = prepare_dataframe(epi_connection, filename)
    upload_df(pho_connection, 'detector_mask_info', df)
    file_df = append_filename(filename_end)
    upload_df(pho_connection, 'uploaded_filenames', file_df)
    close_connection(pho_connection, pho_engine)
    print('Successfully Uploaded')

    
def add_wafer_to_relations_table(wafername,  username, target_ip, target_db, dev_type, password,epi_username, epi_server, epi_db, epi_password):
    pho_connection, pho_engine = define_connection( username, target_ip, target_db, password)
    epi_connection, epi_ingine = define_connection(epi_username, epi_server, epi_db, epi_password)
    wafer_id = get_wafer_id(epi_connection,wafername)
    query = f"SELECT * FROM relations_table WHERE wafer_id='{wafer_id}'"
    sql_object = pho_connection.execute(query)
    id_check = 'none'
    for row in sql_object:
        id_check = row[0]
    if id_check != 'none':
        print('Wafer previously uploaded')
        return 0

    query = f"SELECT device_mask_id FROM epi_wafer WHERE wafer_id = '{wafer_id}'"
    sql_object = epi_connection.execute(query)
    for row in sql_object:
        mask_id = row[0]
    if dev_type=='Detector':
        query = f"SELECT * FROM detector_mask_info WHERE mask_id='{mask_id}'"
        mask_df = pd.read_sql(query, pho_connection)
        
        if mask_df.empty==False:
            relations_df = pd.DataFrame({"wafer_id": wafer_id,"part_id":mask_df['detector_part_id']})

        else:
            print('Mask not in Database!')
    upload_df(pho_connection, 'relations_table', relations_df)
    close_connection(pho_connection, pho_engine)
    print('Successfully Uploaded')

def upload_detector_LIV_files(username, target_ip, target_db, password,epi_username, epi_server, epi_db, epi_password):
    directory = get_directory_user_prompt('C:', 'Select Uploads Directory:')
    filepaths = retrieve_detector_files_from_directory(directory)
    pho_connection, pho_engine = define_connection( username, target_ip, target_db, password)
    epi_connection, epi_ingine = define_connection(epi_username, epi_server, epi_db, epi_password)
    files_to_upload = create_list_to_upload(filepaths,pho_connection)
    for filepath in files_to_upload:
        try:

            filename = os.path.basename(filepath)
            wafer_name = parse_filepath(filepath)
            params_df,data_df = create_detector_liv_dataframes(pho_connection,epi_connection,filepath,wafer_name)
            upload_df(pho_connection, 'detector_measurement_parameters', params_df)
            sweep_id = get_detector_liv_measurement_id(pho_connection)
            data_df['detector_sweep_measurement_id'] = sweep_id
            upload_df(pho_connection, 'detector_measurement_liv_data', data_df)
            file_df = append_filename(filename)
            upload_df(pho_connection, 'uploaded_filenames', file_df)
            print(filename + ' Successfully Uploaded')
        except:
            print("There was an error uploading file '{filename}'")
    close_connection(pho_connection, pho_engine)

def upload_detector_two_point_files(username, target_ip, target_db, password,epi_username, epi_server, epi_db, epi_password):
    directory = get_directory_user_prompt('C:', 'Select Uploads Directory:')
    filepaths = retrieve_two_point_files_from_directory(directory)
    pho_connection, pho_engine = define_connection( username, target_ip, target_db, password)
    epi_connection, epi_ingine = define_connection(epi_username, epi_server, epi_db, epi_password)
    files_to_upload = create_list_to_upload(filepaths,pho_connection)
    for filepath in files_to_upload:
        try:
            filename = os.path.basename(filepath)
            wafer_name = parse_filepath(filepath)
            df = create_two_point_dataframe(filepath, wafer_name, pho_connection, epi_connection)
            upload_df(pho_connection, 'detector_two_point_data', df)
            file_df = append_filename(filename)
            upload_df(pho_connection, 'uploaded_filenames', file_df)
            print(filename + ' Successfully Uploaded')
        except:
            print("There was an error uploading file '{filename}'")
    close_connection(pho_connection, pho_engine)



