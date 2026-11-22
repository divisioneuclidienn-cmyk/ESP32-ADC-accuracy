# Ecrit dans un fichier les données reçues depuis le port USB
# Le programme n'écrit que les données suivant la balise de texte "CSV_START"
# et s'arrête après avoir détecté le texte de balise "CSV_END"
# Note : CTRL + c pour arrêter le programme à tout moment

import serial
import time
import datetime

csv_start_header = "CSV_START"
csv_end_header = "CSV_END"

file_path = "output.txt"

port_name = "COM5"
port = serial.Serial()
port.port = port_name
port.baudrate = 115200
port.dtr = False
port.rts = False

port.open()
port.reset_input_buffer()

response = input("Press enter to start DAQ")
while response != "":
  response = input("Press enter to start DAQ")

with open(file=file_path, mode="a", encoding="UTF-8") as file:  
  try:

    port.reset_input_buffer()
    while True:
      serial_output = port.readline()
      serial_output_str = serial_output.decode("UTF-8", errors="ignore").rstrip("\r\n")
      print(serial_output_str)

      if serial_output_str == "CSV_START":
        # afficher l'horodatage de l'acquisition
        #now = datetime.datetime.now()
        #file.write("#" + str(now) + "\n")
        while True:
          serial_output = port.readline()
          serial_output_str = serial_output.decode("UTF-8", errors="ignore").rstrip("\r\n")

          if serial_output_str == "CSV_END":
            port.close()
            file.close()
            
            print("Fichier enregistré")
            print("Arrêt du programme")
            exit()

          file.write(serial_output_str + "\n")
          file.flush()

          time.sleep(0.001)

      time.sleep(0.001)

  except KeyboardInterrupt:
    print("Arrêt du programme, enregistrement du fichier.")
  finally:
    port.close()
    file.close()