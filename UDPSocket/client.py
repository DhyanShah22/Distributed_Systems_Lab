import socket

def main():
    HOST = '127.0.0.1'
    PORT = int(input("Enter the port to connect the client"))

    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("Connected to UDP server")

    while True:
        msg = input("You: ")
        client.sendto(msg.encode(), (HOST, PORT))
        reply, _ = client.recvfrom(1024) 
        reply = reply.decode()
        print(f"[SERVER]: {reply}")
        if reply == "Bye":
            break
    
    print("[DISCONNECTDE]")
    client.close()

if __name__ == "__main__":
    main()  