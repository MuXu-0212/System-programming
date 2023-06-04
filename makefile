src = $(wildcard ./*.c)
obj = $(patsubst ./%.c, ./%.o, $(src))
gg = gcc  #c语言编译


myArgs = -Wall -g

ALL:a.out

a.out:$(obj)
	$(gg) $^ -o $@ $(myArgs)

$(obj):%.o:%.c
	$(gg) -c $< -o $@ $(myArgs)

clean:
	-rm -rf $(obj) a.out

.PHONY: clean ALL


