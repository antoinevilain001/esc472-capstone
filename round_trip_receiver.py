import socket 

  

# Set up the UDP socket for receiving 

receiver_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 

  

# Define the IP and port on which the receiver will listen 

receiver_address = ('0.0.0.0', 5000)  # Listen on all available interfaces 

  

# Bind the socket to the address 

receiver_socket.bind(receiver_address) 

  

print("Receiver is waiting for messages...") 

  

while True: 

    # Receive data from sender 

    data, address = receiver_socket.recvfrom(1024)  # Buffer size of 1024 bytes 

    print(f"Received message: {data} from {address}") 

  

    # Send the response back to the sender (acknowledgment) 

    receiver_socket.sendto(data, address)  # Respond with the same message (or any other) 