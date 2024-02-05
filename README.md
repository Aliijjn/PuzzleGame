# Welcome to PuzzleGame!

## What is PuzzleGame?👾

PuzzleGame is a 2D game about a blue blob. Your goal is to get to the exit of each level and complete the game. Sounds easy, but there's a catch. You're in space and once you move, you cannot stop or change direction until you hit an obstacle. These obstacles vary in strength and weight, allowing you to interact with your environment.

Screenshots can be found below :D

## Controls:

**WASD:**   move character.

**Escape:**   exit game.

**Shift:**   load next level.

**Control:**   load previous level

**R:**     reload current level

## Blocks:

**Exit:** I don't think this one needs an explanation

**Brick Wall:** a simple wall you can bounce against: that's all

**Cracked Wall:** a brittle version of the brick wall, breaks after one hit

**Slime Block:** Very bouncy, don't expect to stay on course after hitting this block

**Crate:** It's a regular block, just slightly lighter. Try bumping into this one

**Portal:** Will sent you flying out of another portal with a similar colour

## Technical Shenanigans:

This game is fully written in C using the Windows.h API for window management, user input and pixel management. The sprite support, levels and pretty much everthing else has been made by myself. Use whateve you want yourself :)

Sprites are turned into a pixel array using the function `read_file`, which can then be displayed on the screen using `draw_texture`. Levels are read with a similar function and are transferred from a .txt file into a 2d char array, which can be interpreted by the rest of the game logic.

An example of my first level as a .txt file:
```. . . . . . . .
. . . . . . B .
. . B . . . . .
. . . . . . . .
. . . . . . . .
. . . E . . . .
. S . . . . . B
. . . . . . . .
```
This example only features a small amount of characters: `.` for an empty space, `S` for the player spawn `E` for the exit, finally `B` represents a normal brick wall. 

## Screenshots:

Here's the same level in-game
![image](https://github.com/Aliijjn/PuzzleGame/assets/114729493/38092735-a6e3-4ae7-a7f3-6460f37b8363)

Things are getting more complicated 👀
![image](https://github.com/Aliijjn/PuzzleGame/assets/114729493/a6d59ebc-247a-46f5-89ac-fc328c9c8c2c)

The first level featuring crates
![image](https://github.com/Aliijjn/PuzzleGame/assets/114729493/805b275b-c841-4004-9f53-3f8143cc9996)

Portals
![image](https://github.com/Aliijjn/PuzzleGame/assets/114729493/6af6abc6-3e1d-4981-b294-e6479188da5e)

?????

![image](https://github.com/Aliijjn/PuzzleGame/assets/114729493/92bc2099-27ff-4bf6-9779-8acaf07393ba)
