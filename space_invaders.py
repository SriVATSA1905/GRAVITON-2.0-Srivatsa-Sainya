import pygame
import random
import math

# Initialize Pygame
pygame.init()

# Set up the screen
screen_width = 1000
screen_height = 800
screen = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("Space Invaders")

# Load game assets
playerImage = pygame.image.load('Hero.png')
player_width, player_height = playerImage.get_width(), playerImage.get_height()
player_X = (screen_width - player_width) // 2
player_Y = screen_height - player_height - 20
player_Xchange = 5.0  # Increase hero's movement speed

bulletImage = pygame.image.load('Bullet.png')
bullets = []  # List to store bullet coordinates
bullet_Ychange = 10  # Bullet speed

invaderImage = pygame.image.load('Villian.png')
invader_width, invader_height = invaderImage.get_width(), invaderImage.get_height()
no_of_invaders = 5  # Set fewer villains

invader_X = []
invader_Y = []
invader_Ychange = 3.0  # Villain's vertical speed

for _ in range(no_of_invaders):
    invader_X.append(random.randint(64, screen_width - invader_width - 64))
    invader_Y.append(random.randint(30, 180))

# Score
hero_score_val = 0  # Hero's score (villains shot)
villain_score_val = 0  # Villain's score (hero crossing)
font = pygame.font.Font('freesansbold.ttf', 32)
scoreX_hero = 10  # X position for hero's score display
scoreX_villain = screen_width - 200  # X position for villain's score display
scoreY = 10

# Game over text
game_over_font = pygame.font.Font('freesansbold.ttf', 64)

def show_scores():
    hero_score_text = font.render("Hero: " + str(hero_score_val), True, (255, 255, 255))
    villain_score_text = font.render("Villain: " + str(villain_score_val), True, (255, 255, 255))
    screen.blit(hero_score_text, (scoreX_hero, scoreY))
    screen.blit(villain_score_text, (scoreX_villain, scoreY))

def game_over():
    game_over_text = game_over_font.render("GAME OVER", True, (255, 255, 255))
    screen.blit(game_over_text, (screen_width//2 - 200, screen_height//2 - 32))

def player(x, y):
    screen.blit(playerImage, (x, y))

def invader(x, y):
    screen.blit(invaderImage, (int(x), int(y)))  # Ensure coordinates are integers

def fire_bullet(x, y):
    bullets.append([x + player_width//2, y])  # Add bullet coordinates to the list

def isCollision(x1, y1, x2, y2):
    distance = math.sqrt((x1 - x2)**2 + (y1 - y2)**2)
    if distance <= 60:
        return True
    else:
        return False

# Game loop
clock = pygame.time.Clock()  # Create Pygame clock object
running = True
fire_bullet_event = pygame.USEREVENT + 1
pygame.time.set_timer(fire_bullet_event, 200)  # Fire bullet event every 200 milliseconds
while running:
    screen.fill((0, 0, 0))

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == fire_bullet_event:
            fire_bullet(player_X, player_Y)

    # Handle player movement
    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT]:
        player_X -= player_Xchange
    if keys[pygame.K_RIGHT]:
        player_X += player_Xchange

    # Keep player within screen boundaries
    if player_X <= 0:
        player_X = 0
    elif player_X >= screen_width - player_width:
        player_X = screen_width - player_width

    # Update invader positions and draw on screen
    for i in range(no_of_invaders):
        invader(invader_X[i], invader_Y[i])

        # Move villains straight down
        invader_Y[i] += invader_Ychange

        # Check if villain crosses hero
        if invader_Y[i] >= player_Y + player_height:
            villain_score_val += 1  # Increment villain's score when crossing hero
            invader_X[i] = random.randint(64, screen_width - invader_width - 64)
            invader_Y[i] = random.randint(30, 180)

        # Check collisions with bullets
        for bullet in bullets:
            if isCollision(bullet[0], bullet[1], invader_X[i], invader_Y[i]):
                hero_score_val += 1  # Increment hero's score when hitting a villain
                invader_X[i] = random.randint(64, screen_width - invader_width - 64)
                invader_Y[i] = random.randint(30, 180)
                bullets.remove(bullet)
                break

    # Update and draw bullets
    for bullet in bullets:
        bullet[1] -= bullet_Ychange
        screen.blit(bulletImage, (int(bullet[0]), int(bullet[1])))

    # Draw hero and display scores
    player(player_X, player_Y)
    show_scores()

    pygame.display.update()
    clock.tick(60)  # Limit frame rate to 60 FPS

# Quit Pygame
pygame.quit()

