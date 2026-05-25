# Still Early Farm

A C++ 2D farm simulation game built with the CMU Graphics Library (CMUgraphicsLib). This project lets players manage a virtual farm, interact with game objects, and handle a simple budget.

---

## Purpose of the Project

This project was built as an educational assignment to practice software development and Object-Oriented Programming (OOP) concepts. It focuses on several key learning goals:

- Learning OOP Concepts: Putting core programming ideas into practice, such as using inheritance and polymorphism to create different game entities from a base object template.
- Building a Basic Game Framework: Understanding how a standard 2D game works from the ground up, including managing a game loop, handling player inputs, updating object states, and rendering visuals.
- Handling Resource Logic: Implementing game rules that keep track of money and transactions whenever a player buys or interacts with items on the farm.

---

## Features

- Visual 2D Interface: Uses CMUgraphicsLib to create an easy-to-use graphical screen for the game.
- Dynamic Object Management: Systematically tracks and updates different farm elements on the screen automatically.
- Farm Interactions: Includes modular systems specifically designed to handle animals and farm mechanics.
- Live Budget Tracking: Keeps a running tally of in-game funds to enforce budget limits while playing.
- Clean Configuration: Keeps game settings, prices, and rules organized in one central GameConfig.h file.

---

## Project Structure

- Core/ — Contains the main game logic, loop, and base object templates.
- CMUgraphicsLib/ — The graphics library used to draw shapes, images, and text on the screen.
- Config/ — Holds the configuration files for game settings and values.
- Debug/ — Stores temporary build files created during compilation.

---

## Getting Started

### Prerequisites
- Windows OS (Required by the graphics library)
- Visual Studio (Community or Professional edition)

### How to Run
1. Download the repository as a ZIP file.
2. Unzip the folder on your computer.
3. Open the main solution file named graphics_prj.sln using Visual Studio.
4. Press F5 (or click the Start button) to compile and launch the game.

---

## License

This project is intended for educational purposes. Please check the CMUgraphicsLib documentation regarding permissions for using the graphics library.
