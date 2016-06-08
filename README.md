## VAYVED
or Vayve daemon is a service written for playing with the indicator LED of Xiaomi MiWiFi mini router running OpenWRT.
This basically checks(polls) internet connectivity and if connected, starts openvpn as a child process and creates a VPN tunnel to Vayve's OpenVPN Server.

This assumes a precompiled openvpn executable and the openvpn configuration named client_SG_Elastic_IP.ovpn both in /root/bin/


## Code Example
The main daemon is vayved_fsm.c which, called so, because it realizes a finite states machine which the router obeys in this process.

## Motivation
Lots and lots of boredom. Awesome guys @VayveTech

## Installation
You need the MIPS toolchain of OpenWRT to compile this for you MiWiFi router.
For how to flash OpenWRT on Xiomi MiWiFi and get the toolchain for compilation of above follow this link:
https://wiki.openwrt.org/toh/xiaomi/mini


## License
This is licensed under GPL v2. 
Follow this link for more details: http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html
THIS COMES WITH ABSOLUTELY NO WARRENTY. 
USE IT AT YOUR OWN RISK.
