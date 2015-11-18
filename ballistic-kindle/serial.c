
#include "serial.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

t_serial	*serial_new()
{
  t_serial	*res;

  res = malloc(sizeof(t_serial));
  if (!res)
    {
      return (NULL);
    }
  res->port_is_open = 0;
  res->baud_rate = 38400;
  res->tx = 0;
  res->rx = 0;
  return (res);
}

int	serial_open(t_serial *serial, char *name)
{
	int s = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (s < 0) {
	  printf(" no socket\n");
	}

	struct sockaddr_l2 bind_addr = { 0 };
	bind_addr.l2_family = AF_BLUETOOTH;
	bind_addr.l2_cid = htobs(4); // ATT CID
	bacpy(&bind_addr.l2_bdaddr, BDADDR_ANY);
	bind_addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;

	int err = bind(s, (struct sockaddr*)&bind_addr, sizeof(bind_addr));
	if (err) {
	  perror("L2CAP bind failed\n");
	}

	struct sockaddr_l2 conn_addr = { 0 };
	conn_addr.l2_family = AF_BLUETOOTH;
	conn_addr.l2_cid = htobs(4); // ATT CID
	str2ba("D0:39:72:C4:DC:A5", &conn_addr.l2_bdaddr );
	conn_addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;

	err = connect(s, (struct sockaddr*)&conn_addr, sizeof(conn_addr));
	serial->port_fd  = s;
	fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
	if (err) {
		perror("L2CAP connect failed\n");
	}
	serial->port_name = name;
	serial->port_is_open = 1;
	return 0;
}

int serial_setBaud(t_serial *serial, int baud)
{
	/*
  speed_t spd;
  switch (baud) {
  case 230400:    spd = B230400;  break;
  case 115200:    spd = B115200;  break;
  case 57600:     spd = B57600;   break;
  case 38400:     spd = B38400;   break;
  case 19200:     spd = B19200;   break;
  case 9600:      spd = B9600;    break;
  case 4800:      spd = B4800;    break;
  case 2400:      spd = B2400;    break;
  case 1800:      spd = B1800;    break;
  case 1200:      spd = B1200;    break;
  case 600:       spd = B600;     break;
  case 300:       spd = B300;     break;
  case 200:       spd = B200;     break;
  case 150:       spd = B150;     break;
  case 134:       spd = B134;     break;
  case 110:       spd = B110;     break;
  case 75:        spd = B75;      break;
  case 50:        spd = B50;      break;
#ifdef B460800
  case 460800:    spd = B460800;  break;
#endif
#ifdef B500000
  case 500000:    spd = B500000;  break;
#endif
#ifdef B576000
  case 576000:    spd = B576000;  break;
#endif
#ifdef B921600
  case 921600:    spd = B921600;  break;
#endif
#ifdef B1000000
  case 1000000:   spd = B1000000; break;
#endif
#ifdef B1152000
  case 1152000:   spd = B1152000; break;
#endif
#ifdef B1500000
  case 1500000:   spd = B1500000; break;
#endif
#ifdef B2000000
  case 2000000:   spd = B2000000; break;
#endif
#ifdef B2500000
  case 2500000:   spd = B2500000; break;
#endif
#ifdef B3000000
  case 3000000:   spd = B3000000; break;
#endif
#ifdef B3500000
  case 3500000:   spd = B3500000; break;
#endif
#ifdef B4000000
  case 4000000:   spd = B4000000; break;
#endif
#ifdef B7200
  case 7200:      spd = B7200;    break;
#endif
#ifdef B14400
  case 14400:     spd = B14400;   break;
#endif
#ifdef B28800
  case 28800:     spd = B28800;   break;
#endif
#ifdef B76800
  case 76800:     spd = B76800;   break;
#endif
  default: {
    return -1;
  }
  }
  cfsetospeed(&(serial->settings), spd);
  cfsetispeed(&(serial->settings), spd);
  if (tcsetattr(serial->port_fd, TCSANOW, &(serial->settings)) < 0)
    return (-1);
    */
  return (0);
}

int		serial_read(t_serial *serial, void *ptr, int count)
{
  int		n;

  if (!serial->port_is_open)
    return (-1);
  if (count <= 0)
    return (0);
  n = read(serial->port_fd, ptr, count);
  if (n < 0 && (errno == EAGAIN || errno == EINTR))
    return (0);
  serial->rx += n;
  return (n);
}

int		serial_write(t_serial *serial, void *ptr, int len)
{
  if (!serial->port_is_open) return -1;
  int n, written=0;
  fd_set wfds;
  struct timeval tv;
  while (written < len) {
    n = write(serial->port_fd, (const char *)ptr + written, len - written);
    if (n < 0 && (errno == EAGAIN || errno == EINTR)) n = 0;
    printf("Write, n = %d\n", n);
    if (n < 0) return -1;
    if (n > 0) {
      written += n;
    } else {
      tv.tv_sec = 10;
      tv.tv_usec = 0;
      FD_ZERO(&wfds);
      FD_SET(serial->port_fd, &wfds);
      n = select(serial->port_fd, NULL, &wfds, NULL, &tv);
      if (n < 0 && errno == EINTR) n = 1;
      if (n <= 0) return -1;
    }
  }
  serial->tx += written;
  return (written);
}

int		serial_waitInput(t_serial *serial, int msec)
{
  if (!serial->port_is_open) return -1;
  fd_set rfds;
  struct timeval tv;
  tv.tv_sec = msec / 1000;
  tv.tv_usec = (msec % 1000) * 1000;
  FD_ZERO(&rfds);
  FD_SET(serial->port_fd, &rfds);
  return (select(serial->port_fd+1, &rfds, NULL, NULL, &tv));
}


int		serial_setControl(t_serial *serial, int dtr, int rts)
{
	/*
  if (!serial->port_is_open) return -1;
  int bits;
  if (ioctl(serial->port_fd, TIOCMGET, &bits) < 0) return -1;
  if (dtr == 1) {
    bits |= TIOCM_DTR;
  } else if (dtr == 0) {
    bits &= ~TIOCM_DTR;
  }
  if (rts == 1) {
    bits |= TIOCM_RTS;
  } else if (rts == 0) {
    bits &= ~TIOCM_RTS;
  }
  if (ioctl(serial->port_fd, TIOCMSET, &bits) < 0) return -1;;
  */
  return (0);
}
