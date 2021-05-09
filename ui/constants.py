from game.constants import PLAYER_1_ID, PLAYER_2_ID, PLAY_AREA_SIZE

# DISPLAY SIZING
SIZE = WIDTH, HEIGHT = 800, 600
DX = WIDTH * 0.7 / PLAY_AREA_SIZE
DY = HEIGHT * 0.9 / PLAY_AREA_SIZE
TILES_BORDER = 5

# MATH
SQRT_2 = 1.414213

# COLORS
TILE_COLOR = 170, 170, 170
PERIMETER_TILE_COLOR = 220, 220, 220
BACKGROUND_COLOR = 255, 255, 255
PLAYER_COLOR = {
    PLAYER_1_ID: [50, 180, 80],
    PLAYER_2_ID: [50, 80, 180]
}
