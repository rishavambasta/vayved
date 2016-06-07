#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#define MAX_LINE 500

void report_error (char message[])
{
  printf ("\n%s",message);
}

bool internetConnected ()
{
  int sockfd,val;
  char buffer[MAX_LINE];
  struct sockaddr_in ipAddress;
  struct hostent *entity=NULL;

  sockfd = -1;

  if((entity = gethostbyname("www.google.com")) != NULL)
    {
      if((sockfd = socket(entity->h_addrtype,SOCK_STREAM,IPPROTO_TCP)) != -1)
        {
          val = 1;
          if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (char *) &val, sizeof(val)) == 0 && setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY, (char *) &val, sizeof(val)) == 0)
            {
              ipAddress.sin_family = entity->h_addrtype;
              memcpy(&(ipAddress.sin_addr), entity->h_addr, entity->h_length);
              ipAddress.sin_port = htons(80);

              if(connect(sockfd,(struct sockaddr *) &ipAddress,sizeof(ipAddress)) == 0)
                {
                  if(write(sockfd,"GET /index.html HTTP/1.1\r\n\r\n", 29) >= 28)
                    {
                      shutdown(sockfd, SHUT_WR);
                      if(read(sockfd, buffer, MAX_LINE) != -1) // all right!
                        {
                          close(sockfd);
                          return true;
                        }
                      else
                        report_error("read()");
                    }
                  else
                    report_error("write()");
                }
              else
                report_error("connect()");
            }
          else
            report_error("setsockopt()");
        }
      else
        report_error("socket()");
    }
  else
    report_error("cannot resolve IP for Google."); // this is is the most common error.

  if(sockfd!=-1)
    close(sockfd); // no internet
  return false;
}
