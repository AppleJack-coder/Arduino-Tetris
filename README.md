# Arduino-Tetris
Tetris game using components from popular arduino kits

# Components list

## Project itself
1) Led matrix 8x8 (one colored) without driver (1088AGG)
2) 8-bit shift register 74HC595
3) Matrix shield (schematics are in a "shield" folder)
4) 4x 10k hOm resistor 
4) 4x push buttons
5) Arduino Uno r3 (or any other)
6) 20x wires

Optional:
7) IR receiver module
8) IR remote

## Matrix shield
1) Materials needed to make PCB
2) 8x 0hOm smd resistors (0603)
3) 24x 1k hOm smd resistors (0402)

# Schematics

## Matrix pinout and shield
![Matrix pinout](https://github.com/AppleJack-coder/Arduino-Tetris/blob/main/schematics/matrix_pinout.png)
![Matrix shield](https://github.com/AppleJack-coder/Arduino-Tetris/blob/main/schematics/matrix_shield.png)

## Breadboard with buttons
![Breadboard with buttons](https://github.com/AppleJack-coder/Arduino-Tetris/blob/main/schematics/tetris_schematics_with_buttons.png)


# Results

## Breadboard with IR remote
![Breadboard with IR remote](https://github.com/AppleJack-coder/Arduino-Tetris/blob/main/schematics/breadboard_with_IR_remote.jpg)

# TODO list
- [x] 1. Func to display any symbol on led matrix
- [x] 2. Func to move symbol using code
- [ ] 3. Func for rotation
- [x] 4. Add constraits
- [x] 5. Add controls
- [x] 6. Add gravity
- [x] 7. Add blocks generation
- [x] 8. Add piling up of tetris pieces
- [x] 9. Add deletion of whole row
- [x] 10. Add death on overflow
- [ ] 11. Add levels
- [ ] 12. Add starting screen and finish score
- [ ] 13. Count bits using lookup table
- [ ] 14. Fix bug with random block generator
- [ ] 15. Not all blocks are falling when row is deleted