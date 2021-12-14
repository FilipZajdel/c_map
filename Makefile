target = cmap

srcs = cmap.c \
	example.c \

objs = $(srcs:%.c=%.o)

inc = \
	-I.

all: ${objs}
	gcc ${objs} -o ${target}

%.o: %.c
	gcc -c $< -o $@ ${inc}

clean:
	rm -rf *.o ${target}
