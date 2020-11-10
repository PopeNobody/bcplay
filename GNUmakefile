ifeq ($(MAKECMDGOALS),)

include etc/default_target.mk

%:
	cleanmake -f Makefile

else

$(MAKECMDGOALS): 
	cleanmake -f Makefile $(MAKECMDGOALS)

endif

