import React, { useState, useEffect } from 'react';
import io from 'socket.io-client';
import Message from './Message';

function Chat({ token }) {
  const [message, setMessage] = useState('');
  const [messages, setMessages] = useState([]);
  const socket = io(import.meta.env.VITE_SOCKET_URL);

  useEffect(() => {
    socket.emit('joinChat', { token });

    socket.on('message', (msg) => {
      setMessages((prevMessages) => [...prevMessages, msg]);
    });

    return () => {
      socket.disconnect();
    };
  }, [token]);

  const sendMessage = (e) => {
    e.preventDefault();
    if (message.trim()) {
      socket.emit('sendMessage', { token, message });
      setMessage('');
    }
  };

  return (
    <div className="container">
      <div className="chat-container">
        {messages.map((msg, index) => (
          <Message key={index} text={msg.text} isMe={msg.user === token} />
        ))}
      </div>
      <form onSubmit={sendMessage}>
        <input
          type="text"
          placeholder="Enter your message..."
          value={message}
          onChange={(e) => setMessage(e.target.value)}
          required
        />
        <button type="submit">Send</button>
      </form>
    </div>
  );
}

export default Chat;
