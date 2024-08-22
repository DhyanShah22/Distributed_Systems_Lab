import React from 'react';

function Message({ text, isMe }) {
  return (
    <div className={`message ${isMe ? 'me' : ''}`}>
      {text}
    </div>
  );
}

export default Message;
