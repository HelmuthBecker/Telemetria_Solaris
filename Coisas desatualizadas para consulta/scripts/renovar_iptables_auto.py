#!/bin/python3

import time
import os

while True:
    os.system("sudo /home/pi/Documents/scripts/regras_iptables.sh")
    #Envia um comando para executar o script que irá resetar as regras iptbles
    time.sleep(120)
    #Aguarda 2 min e executa o script novamente