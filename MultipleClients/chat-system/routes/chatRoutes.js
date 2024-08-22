const express = require('express');
const router = express.Router();
const { initializeChat } = require('../controllers/chatController');

// Initialize chat with Socket.io
router.post('/initialize', (req, res) => {
  // This route can be used to initialize chat if needed in the future
  res.send('Chat initialization route');
});

module.exports = router;
