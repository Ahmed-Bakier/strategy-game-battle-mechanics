
# Strategy Game Battle Mechanics

## Description
This project simulates a grid-based strategy game where two factions, the Human Empire and the Orc Legion, battle using units, heroes, and creatures. The battle mechanics include attack/defense calculations, critical hits, health management, and fatigue effects. The game reads data from JSON files to define unit stats, hero bonuses, creature effects, and research upgrades. The simulation is visualized on a 20x20 grid, with health bars and effects for heroes and creatures.

## Features
- **Grid-Based Battle**: The battle takes place on a 20x20 grid, with units distributed across the grid.
- **JSON Data Processing**: The game reads data from four JSON files (`unit_types.json`, `heroes.json`, `creatures.json`, `research.json`) to define unit stats, hero bonuses, creature effects, and research upgrades.
- **Battle Mechanics**: The simulation calculates attack power, defense power, critical hits, health, fatigue, and other factors to determine the outcome of each battle step.
- **Visualization**: The battle is visualized using a grid system, with health bars and effects for heroes and creatures.
- **Dynamic Scenario Loading**: The game loads scenarios dynamically from predefined URLs.

## Technologies Used
- **Programming Language**: C
- **Data Formats**: JSON
- **Visualization**: Grid-based system with health bars and effects.

## How to Run
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/strategy-game-battle-mechanics.git