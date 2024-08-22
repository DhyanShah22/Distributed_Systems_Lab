const express = require('express');
const http = require('http');
const socketio = require('socket.io');
const connectDB = require('./config/db');
const authRoutes = require('./routes/authRoutes');
const chatRoutes = require('./routes/chatRoutes'); // Make sure this import is correct
const { initializeChat } = require('./controllers/chatController');
require('dotenv').config();
const cors = require('cors');

const app = express();
const server = http.createServer(app);
const io = socketio(server);

// Connect to MongoDB
connectDB();

// Middleware
app.use(express.json());
app.use(cors());

// Routes
app.use('/api/auth', authRoutes);
app.use('/api/chat', chatRoutes); // Use chat routes here

// Initialize Chat with Socket.io
initializeChat(io);

// Start Server
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => console.log(`Server running on port ${PORT}`));
