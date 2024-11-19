# import socket

# def main():
#     HOST = '127.0.0.1'
#     PORT = int(input("Enter port to run the server: "))

#     server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#     server.bind((HOST, PORT))
#     print(f"Server listening on {HOST}:{PORT}")

#     while True:
#         msg, addr = server.recvfrom(1024)
#         msg = msg.decode()
#         print(f"Message from {addr}: {msg}")

#         if msg.lower() == 'bye':
#             server.sendto("Bye".encode(), addr)
#             print(f"[DISCONNECTED] Client {addr}")
#             break

#         reply = msg.upper()
#         server.sendto(reply.encode(), addr)

#     server.close()

# if __name__ == '__main__':
#     main()


import socket

def main():
    HOST = '127.0.0.1'
    PORT = int(input("Enter the port to bind the server: "))
    
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind((HOST, PORT))
    print(f"Server listening on {HOST}:{PORT}")

    while True:
        msg, addr = server.recvfrom(1024)
        msg = msg.decode()
        print(f"Message from {addr}: {msg}")

        if msg.lower() == 'bye':
            server.sendto("Bye".encode(), addr)
            print(f"[DISCONNECTED] Client {addr}")
            break

        reply = msg.upper()
        server.sendto(reply.encode(), addr)

    server.close()

if __name__ == '__main__':
    main()