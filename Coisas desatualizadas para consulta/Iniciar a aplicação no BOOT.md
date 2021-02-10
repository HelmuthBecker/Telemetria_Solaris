1 - CRIAR SCRIPT PARA INICIAR O SERVIDOR NODE JS


1.1 - Abra o terminal e navegue até a pasta onde o arquivo criado será salvo (Desktop por exemplo)

1.2 - Na janela do terminal digite: 
      
      sudo nano iniciar_NPM.sh

1.3 - Na janela que se abrir insira o seguinte texto: 

      !/bin/bash
      cd /home/pi/Desktop/app_solaris
      npm start

1.4 - salve e feche o arquivo (ctrl + x / y)

1.5 - Após fechar o arquivo "iniciar_NPM.sh", ainda com o terminal aberto 
      e na mesma pasta do arquivo, insira o seguinte comando: 

      sudo chmod +x iniciar_NPM.sh

**Isto irá tornar o script um arquivo executavel

1.6 - Abra o arquivo rc.local. Para isso insira o seguinte comando: 
      
      sudo nano /etc/rc.local

1.7 - Vá até o fim do arquivo, e na linha acima do texto "exit 0", insira o seguinte

      sudo bash /home/pi/Desktop/start_NPM.sh

**Isto irá fazer com que o script inicie junto com o sistema

1.8 - salve e feche o arquivo (ctrl + x / y)

2 - INICIANDO A PAGINA DA APLICAÇÃO COM O SISTEMA

* Caso o chromium não seja encerrado corretamente, ao iniciar ele irá exibir uma mensagem
perguntando se o usuario gostaria de restaurar as janelas que estavam abertas anteriormente.
Isto pode impedir que a janela com o mapa da aplicação apareça automaticamente.
Para evitar isso, faça o seguinte.


2.1 - Na janela do terminal digite: 
      
      sudo nano /home/pi/.config/chromium/Default/Preferences

*Isto irá abrir o arquivo preferences do chromium

2.2 - Procure por um trecho do arquivo que deve estar escrito desta forma:

	"exit_type":"Normal","exited_cleanly":true

2.3 - Caso ja esteja como está acima, não modifique e feche o arquivo (ctrl + x). Caso contrario,
modifique para que fique como o texto acima, feche e salve (ctrl + x / y)

2.4 - Após conferir que o arquivo está correto, o que iremos fazer é criar uma cópia de segurança
deste arquivo, salvar em alguma pasta e configurar o sistema para substituir o arquivo salvo,
pelo arquivo presente na pasta do chromium, toda vez que iniciar o sistema (raspbian)

2.5 - Para isso, no terminal, insira o seguinte comando:

	mkdir /home/pi/Desktop/chromium_prefs

*Isso irá criar uma pasta chromium_prefs na área de trabalho

2.6 - Após, insira o seguinte comando

	cp /home/pi/.config/chromium/Default/Preferences /home/pi/Desktop/chromium_prefs  

*Isso irá copiar o arquivo da pasta Preferences do chromium, para a pasta chromium_prefs

2.7 - Agora iremos criar o script que irá fazer o sistema substituir o arquivo e abrir a pagina
da aplicação. No terminal insira:

	sudo nano start.sh

*Isto irá criar um arquivo em branco na pasta chromium_prefs 
(caso você esteja na pasta chromium_prefs no terminal)

2.8 - Dentro do arquivo insira o seguinte:

	#!/usr/bin/sh

	cp /home/pi/chromium_prefs/Preferences /home/pi/.config/chromium/Default
	chromium-browser --kiosk http://localhost:3000/aplicacao
	exit 0

2.9 - Salve e feche o arquivo (ctrl + x / y)

2.10 - Após fechar o arquivo "start.sh", ainda com o terminal aberto 
      e na mesma pasta do arquivo, insira o seguinte comando: 

	sudo chmod +x start.sh

**Isto irá tornar o script um arquivo executavel

2.11 - Abra o arquivo autostart:

	sudo nano /home/pi/.config/lxsession/LXDE-pi/autostart

2.12 - Navegue até o fim do arquivo e insira o seguinte:

      @xscreensaver -no-splash
      @xset s off
      @xset -dpms
      @xset s noblank
      @sh /home/pi/Desktop/chromium_prefs/start.sh

2.13 - salve e feche o arquivo (ctrl + x / y)

2.14 - Reinicie o sistema 