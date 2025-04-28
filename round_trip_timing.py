import socket 

import time 

  

print("Starting Program") 

# Set up the UDP socket 

sender_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 

  

# Define the server address and port (receiver side) 

server_address = ('192.168.1.100', 5000)  # Receiver's IP and port 

  

# Create a message 

message = b'Hello, Receiver!' 

  

# Record the start time 

start_time = time.perf_counter() 

  

# Send the UDP packet 

sender_socket.sendto(message, server_address) 

  

# Receive the response (acknowledgment) from the receiver 

data, _ = sender_socket.recvfrom(1024)  # Buffer size of 1024 bytes 

  

# Record the end time 

end_time = time.perf_counter() 

  

# Calculate the round-trip time (RTT) 

rtt = (end_time - start_time) * 1_000_000  # Convert to microseconds 

  

print(f"Round-trip time: {rtt:.6f} microseconds") 

  

# Close the socket 

sender_socket.close() 