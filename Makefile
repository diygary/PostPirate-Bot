.PHONY: clean All

All:
	@echo "----------Building project:[ TwitterBot - Debug ]----------"
	@"$(MAKE)" -f  "TwitterBot.mk"
clean:
	@echo "----------Cleaning project:[ TwitterBot - Debug ]----------"
	@"$(MAKE)" -f  "TwitterBot.mk" clean
