COMMON += demo_dot demo_mvmul demo_pla demo_pocket_pla demo_pocket_pla_gy kmeans-master

all:
	for x in $(COMMON);do make -C $$x || exit 1;done
	@echo  $(COMMON)

alls:
	for x in $(COMMON);do make -C $$x || exit 1;done 

clean:
	for x in $(COMMON);do make -C $$x clean || exit 1;done 
