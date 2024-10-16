document.getElementById('task-form').addEventListener('submit', function(event) {
    event.preventDefault(); // Prevent the default form submission

    const taskInput = document.getElementById('task');
    const task = taskInput.value;

    fetch('/MyWebApp/task-manager', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: new URLSearchParams({ task: task }) // Use URLSearchParams for form-encoded data
    })
    .then(response => response.text()) // Change to text() since the response is plain text
    .then(data => {
        console.log('Task added:', data);
        taskInput.value = ''; // Clear the input field
        loadTasks(); // Reload the tasks
    })
    .catch(error => {
        console.error('Error:', error);
    });
});

// Function to load tasks from the backend (GET request)
function loadTasks() {
    fetch('/MyWebApp/task-manager')
        .then(response => response.json())
        .then(tasks => {
            const taskList = document.getElementById('task-list');
            taskList.innerHTML = ''; // Clear existing tasks
            tasks.forEach(task => {
                const div = document.createElement('div');
                div.textContent = task;
                taskList.appendChild(div);
            });
        })
        .catch(error => {
            console.error('Error loading tasks:', error);
        });
}

// Call loadTasks on page load
window.onload = loadTasks;
