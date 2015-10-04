
#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam)
{
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}

int main()
{
	//bdaddr_t src_addr, dst_addr;
	const int device = hci_open_dev(hci_get_route(NULL));
	if ( device < 0 ) {
		perror("Failed to open HCI device.");
		return 0;
	}

	bdaddr_t bdaddr;
	uint16_t handle;
	uint16_t interval, latency, max_ce_length, max_interval, min_ce_length;
	uint16_t min_interval, supervision_timeout, window;
	uint8_t initiator_filter, own_bdaddr_type, peer_bdaddr_type;

	own_bdaddr_type = LE_PUBLIC_ADDRESS;
	peer_bdaddr_type = LE_PUBLIC_ADDRESS;

	str2ba("D0:39:72:C4:DC:A5", &bdaddr);

	interval = htobs(0x0004);
	window = htobs(0x0004);
	initiator_filter = 0;
	min_interval = htobs(0x000F);
	max_interval = htobs(0x000F);
	latency = htobs(0x0000);
	supervision_timeout = htobs(0x0C80);
	min_ce_length = htobs(0x0000);
	max_ce_length = htobs(0x0000);

	int err = hci_le_create_conn(device, interval, window, initiator_filter, peer_bdaddr_type, bdaddr,
			own_bdaddr_type, min_interval, max_interval, latency, supervision_timeout, min_ce_length, max_ce_length, &handle, 25000);

	if (err < 0) {
		perror("Could not create connection");
		exit(1);
	}

	usleep(10000000);

	err = hci_disconnect(device, handle, HCI_OE_USER_ENDED_CONNECTION, 10000);
	fprintf(stdout, "hci_disconnect returns %i\n", err);

	err = hci_close_dev(device);
	fprintf(stdout, "%i\n", err);

	exit(0);
}


