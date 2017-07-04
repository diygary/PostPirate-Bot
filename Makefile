.PHONY: clean All

All:
	@echo "----------Building project:[ TwitterBot - Release ]----------"
	@"$(MAKE)" -f  "TwitterBot.mk"
clean:
	@echo "----------Cleaning project:[ TwitterBot - Release ]----------"
	@"$(MAKE)" -f  "TwitterBot.mk" clean
