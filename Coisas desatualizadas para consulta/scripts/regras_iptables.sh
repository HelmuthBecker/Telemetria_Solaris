#!/bin/bash
echo 'RENOVANDO REGRAS IPTABLES'
echo ' '
echo 'LIMPANDO REGRAS TABELA NAT'
sudo iptables -t nat -F
echo 'LIMPANDO REGRAS TABELA FORWARD'
sudo iptables -F
echo 'APLICANDO NOVAMENTE REGRAS IPTABLES..... '
sudo iptables -t nat -A PREROUTING -i eth0 -j DNAT --to 192.168.5.1
sudo iptables -t nat -A POSTROUTING -o eth0 -j SNAT --to 192.168.5.1
sudo iptables -t nat -A  POSTROUTING -o eth0 -j MASQUERADE
sudo iptables -A FORWARD -i eth0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -t nat -A  POSTROUTING -o usb0 -j MASQUERADE
sudo iptables -A FORWARD -i usb0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i usb0 -o eth0 -m state --state RELATED,ESTABLISHED -j ACCEPT
sudo iptables -A FORWARD -i wlan0 -o eth0 -j ACCEPT
sudo iptables -A FORWARD -i wlan0 -o usb0 -j ACCEPT
echo 'REDIRECIONANDO ENDEREÇO E PORTA DA APLICAÇÃO '
sudo iptables -t nat -I PREROUTING -p tcp -s 192.168.5.0/24 -d 192.168.5.1 -j DNAT --to-destination 192.168.5.1:8088
echo 'LIBERANDO PORTA VNC'
sudo iptables -t nat -I PREROUTING -p tcp --dport 5900 -j ACCEPT
echo 'BLOQUEANDO ACESSO A INTERNET VIA WIFI'
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 443 -j DROP
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 80  -j DROP
echo 'BLOQUEANDO PORTAS WHATSAPP'
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 4244  -j DROP
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 5222  -j DROP
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 5223  -j DROP
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 5228  -j DROP
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 5242  -j DROP
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 50318  -j DROP
sudo iptables -I FORWARD -i wlan0 -p tcp --dport 59234  -j DROP
sudo iptables -I FORWARD -i wlan0 -p udp --dport 3478  -j DROP
sudo iptables -I FORWARD -i wlan0 -p udp --dport 45395  -j DROP
sudo iptables -I FORWARD -i wlan0 -p udp --dport 50318  -j DROP
sudo iptables -I FORWARD -i wlan0 -p udp --dport 59234  -j DROP
echo 'LIBERANDO ACESSO A SITES ESSENCIAIS A APLICAÇÃO'
sudo iptables -I FORWARD -i wlan0 -d server.arcgisonline.com -p tcp --dport 443 -j ACCEPT
sudo iptables -I FORWARD -i wlan0 -d use.fontawesome.com -p tcp --dport 443 -j ACCEPT
sudo iptables -I FORWARD -i wlan0 -d maxcdn.bootstrapcdn.com -p tcp --dport 443 -j ACCEPT
sudo iptables -I FORWARD -i wlan0 -d ajax.googleapis.com -p tcp --dport 443 -j ACCEPT
sudo iptables -I FORWARD -i wlan0 -d cdnjs.cloudflare.com -p tcp --dport 443 -j ACCEPT
sudo iptables -I FORWARD -i wlan0 -d unpkg.com -p tcp --dport 443 -j ACCEPT
echo ' '
echo 'REGRAS APLICADAS COM SUCESSO'
echo ' '
echo 'SALVANDO CONFIGURAÇÕES'
sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"
echo ' '
echo 'CONFIGURAÇÕES SALVAS'
echo ' '