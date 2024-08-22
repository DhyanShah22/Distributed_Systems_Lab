const User = require('../models/User');
const jwt = require('jsonwebtoken');

exports.initializeChat = (io) => {
  io.on('connection', (socket) => {
    console.log('New client connected');

    // Join Chat Room
    socket.on('joinChat', async ({ token }) => {
      try {
        // Verify Token
        const decoded = jwt.verify(token, process.env.JWT_SECRET);
        const user = await User.findById(decoded.id);

        // Check if User and Chat Key are Valid
        if (user && user.chatKey === process.env.CHAT_KEY) {
          socket.join('chatroom');
          socket.emit('message', { user: 'admin', text: `${user.username} has joined!` });
        } else {
          socket.disconnect();
        }
      } catch (err) {
        console.error('Error joining chat:', err);
        socket.disconnect();
      }
    });

    // Send Message to Chat Room
    socket.on('sendMessage', ({ token, message }) => {
      try {
        // Verify Token
        const decoded = jwt.verify(token, process.env.JWT_SECRET);
        
        // Emit Message
        io.to('chatroom').emit('message', { user: decoded.id, text: message });
      } catch (err) {
        console.error('Error sending message:', err);
        socket.disconnect();
      }
    });

    // Handle Disconnection
    socket.on('disconnect', () => {
      console.log('Client disconnected');
    });
  });
};
