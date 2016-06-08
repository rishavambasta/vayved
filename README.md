## VAYVED
or Vayve daemon is a service written for playing with the indicator LED of Xiaomi MiWiFi mini router running OpenWRT.
This basically checks(polls) internet connectivity and if connected, starts openvpn as a child process and creates a VPN tunnel to Vayve's OpenVPN Server.
<br/>
In the process, the LED turns </br>
RED(offline) -> BLINKING BLUE(polling internet connectivity) -> BLUE(online) -> BLINKING VIOLET(spawning OpenVPN client) -> VOILET(tunnel created), if everything goes smooth.
<br/>
This assumes a precompiled openvpn executable and the <b>openvpn</b> configuration named <b>client_SG_Elastic_IP.ovpn</b> both in /root/bin/


## Code
The main daemon is <b>vayved_fsm.c</b> which, is called so, because it realizes a finite states machine which the router obeys in this process.

## Motivation
Lots and lots of boredom. And the awesome guys <b>@VayveTech</b>

## Installation
You need the MIPS toolchain of OpenWRT to compile this for you MiWiFi router.<br/>
For how to flash OpenWRT on Xiomi MiWiFi and get the toolchain for compilation of above follow this link:
https://wiki.openwrt.org/toh/xiaomi/mini
<br/> To compile, just run  <b>make</b>


## License
This is licensed under GPL v2.<br/> 
Follow this link for more details: http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html
</br/>IT COMES WITH ABSOLUTELY NO WARRENTY. <br/>
USE IT AT YOUR OWN RISK.
