build:
	@gcc -I/usr/include/ -I./include/ -I./src/ \
		include/cwalk/cwalk.c \
		include/array_list/array_list.c \
		include/buffer/buffer.c \
		src/game/game.c \
		src/game/hud.c \
		src/game/turn.c \
		src/game/level.c \
		src/game/fog.c \
		src/game/log.c \
		src/game/course.c \
		src/game/inter_screen.c \
		src/time/time.c \
		src/figure/figure.c \
		src/figure/movement.c \
		src/figure/combat.c \
		src/figure/city.c \
		src/screen/screen.c \
		src/screen/draw.c \
		src/config/config.c \
		src/animation/animation.c \
		src/input/mouse.c \
		src/input/keyboard.c \
		src/handle/handle.c \
		src/map/loader.c \
		src/ui/ui.c \
		src/ui/notify.c \
		src/map/map.c \
		src/save/save.c \
		src/main/main.c -o thegame -lm -lSDL2 -lSDL2_image -lSDL2_ttf -g \
		-Wall

run: build
	@./thegame /home/andrew/lastballe-saves/default.sav
