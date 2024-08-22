import React, { useState } from 'react';
import Login from '../components/Login';
import Register from '../components/Register';
import Chat from '../components/Chat';

function ChatPage() {
  const [token, setToken] = useState(null);

  if (!token) {
    return (
      <div className="container">
        <Login onLogin={setToken} />
        <Register onRegister={setToken} />
      </div>
    );
  }

  return <Chat token={token} />;
}

export default ChatPage;
