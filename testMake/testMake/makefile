src = $(wildcard ./src/*.c)
obj = $(patsubst ./src/%.c, ./obj/%.o, $(src))

target = keymng_msg
inc_path = ./inc

all:$(target)

$(target):$(obj)
	gcc $^ -o $@

# 静态模式规则
$(obj):./obj/%.o:./src/%.c
	gcc -c $< -o $@ -I $(inc_path)

clean:
	-rm -rf $(obj) $(target)

.PHONY:clean all
