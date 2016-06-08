#ifndef VAYVED_H
#define VAYVED_H
#define INTER_POLL_DELAY 1 //seconds
#define VERBOSE 1

const char tun0_device_file[]="/sys/class/net/tun0";
const char openVPN_binary[] = "/root/bin/openvpn";
const char openVPN_params[] = "--config";
const char ovpn_file_path[] = "/root/bin/client_SG_Elastic_IP.ovpn";

//TODO: implement a state called TUNNELLED_BUT_OFFLINE
//currently its being assumed that tunnelled traffic always findes route to Internet
typedef enum
{
  OFFLINE,
  TRYING_TO_GO_ONLINE,
  ONLINE_UNTUNNELLED,
  TRYING_TO_CREATE_TUNNEL,
  ONLINE_TUNNELLED
}FSM_STATES;


bool internetConnected (void);
bool tunnelExists (void);
int  createTunnel(void);
void resetLeds(void);
void terminate_tunnel(void);
void closeLedDeviceFiles ();
void openLedDeviceFiles ();
#endif // VAYVED_H

