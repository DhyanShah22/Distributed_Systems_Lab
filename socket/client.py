import socket

def main():
    HOST = '127.0.0.1'
    PORT = int(input('Enter the port to connect the client'))

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((HOST, PORT))
    approval_message = client.recv(1024).decode()
    print(approval_message)

    if 'denied' in approval_message.lower():
        print('Connection denied')
    else:
        while True:
            msg = input("You: ")
            client.send(msg.encode())
            reply = client.recv(1024).decode()
            print(f"Server: {reply}")
            if reply == 'Bye':
                break
        
        print("[DISCONNECTED]")
        client.close()


if __name__ == '__main__':
    main()
