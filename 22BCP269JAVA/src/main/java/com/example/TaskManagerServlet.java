package com.example;

import jakarta.servlet.ServletException;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import jakarta.servlet.http.HttpSession;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

@WebServlet("/task-manager")
public class TaskManagerServlet extends HttpServlet {
    private static final long serialVersionUID = 1L;

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        HttpSession session = request.getSession();
        List<String> tasks = getTasksFromSession(session);

        System.out.println("Current tasks: " + tasks); // Debugging output

        response.setContentType("application/json");
        PrintWriter out = response.getWriter();
        out.println("[");
        for (int i = 0; i < tasks.size(); i++) {
            out.println("\"" + tasks.get(i) + "\"" + (i < tasks.size() - 1 ? "," : ""));
        }
        out.println("]");
    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        HttpSession session = request.getSession();
        List<String> tasks = getTasksFromSession(session);

        String newTask = request.getParameter("task");
        if (newTask != null && !newTask.trim().isEmpty()) {
            tasks.add(newTask);  // Add the new task to the list
            session.setAttribute("tasks", tasks);  // Update the session attribute
        }

        // Instead of redirecting, send a response indicating success
        response.setContentType("text/plain");
        response.getWriter().println("Task added: " + newTask);
    }

    @Override
    protected void doPut(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        HttpSession session = request.getSession();
        List<String> tasks = getTasksFromSession(session);

        if (tasks != null) {
            String taskToComplete = request.getParameter("task");
            if (taskToComplete != null && tasks.contains(taskToComplete)) {
                tasks.remove(taskToComplete);
                tasks.add(taskToComplete + " (completed)");
                session.setAttribute("tasks", tasks);
            }
        }
    }

    @Override
    protected void doDelete(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        HttpSession session = request.getSession();
        List<String> tasks = getTasksFromSession(session);

        if (tasks != null) {
            String taskToDelete = request.getParameter("task");
            if (taskToDelete != null) {
                tasks.remove(taskToDelete);
                session.setAttribute("tasks", tasks);
            }
        }
    }

    @SuppressWarnings("unchecked")
    private List<String> getTasksFromSession(HttpSession session) {
        // Check if the session attribute is a List
        Object obj = session.getAttribute("tasks");
        if (obj instanceof List<?>) {
            return (List<String>) obj;  // Suppress the unchecked cast warning here
        } else {
            return new ArrayList<>();  // Return a new list if none exists
        }
    }
}
