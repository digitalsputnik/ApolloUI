import socket

UDP_IP = "192.168.1.248"
UDP_PORT = 6454

msg = """
B000 000w320h054r046g046b046;
B010 010w300h034r079g079b079;
T160 016a1f4r255g255b255r079g079b079Apollo0189;
T030 016a0f4r255g255b255r079g079b079<;
T290 016a2f4r255g255b255r079g079b079>;
B010 064w300h040r046g046b046;
T030 074a0f4r255g213b046r046g046b046Intensity;
T290 074a2f4r079g079b079r046g046b046100%;
B010 114w300h040r046g046b046;
T030 124a0f4r255g213b046r046g046b046Temperature;
T290 124a2f4r079g079b079r046g046b0465600K;
B010 164w300h040r046g046b046;
T030 174a0f4r255g213b046r046g046b046Saturation;
T290 174a2f4r079g079b079r046g046b0460%;
B010 214w300h040r046g046b046;
T030 224a0f4r255g213b046r046g046b046Hue;
T290 224a2f4r079g079b079r046g046b0460%;
B010 264w300h040r046g046b046;
T030 274a0f4r255g213b046r046g046b046Fx;
T290 274a2f4r079g079b079r046g046b040%;
B010 314w300h040r046g046b046;
T030 324a0f4r255g213b046r046g046b046Manage Viewers;"""

def mysend(msg = ""):
    print("UDP target IP:", UDP_IP)
    print("UDP target port:", UDP_PORT)
    print("message:", msg)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    sock.sendto(bytes(msg, "utf-8"), (UDP_IP, UDP_PORT))
    
mysend("Pw001"+msg)
mysend("Pe001")
