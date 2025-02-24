import socket

HOST = "127.0.0.1"
PORT = 65432

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    try:
        s.connect((HOST, PORT))
        s.sendall(b"Tell me about you")
        data = s.recv(1024)

        print(f"Received server message: {data}")

        while True:
            msg = input()
            s.sendall(bytes(msg, 'utf-8'))

            data = s.recv(1024)
            if data:
                print(data.decode('utf-8'))
            else:
                print("Server closed the connection")
                break
            if msg == 'stop' or data.decode('utf-8') == 'Client timeout':
                break
    except ConnectionRefusedError:
        print("Can't connect to server")
    except ConnectionResetError:
        print("Random disconnecting")
    except Exception as e:
        print(f"Other problem: {e}")