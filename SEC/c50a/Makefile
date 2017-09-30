###
# This program was written by and is copyright Alec Muffett 1991,
# 1992, 1993, 1994, 1995, and 1996, and is provided as part of the
# Crack v5.0 Password Cracking package.
#
# The copyright holder disclaims all responsibility or liability with
# respect to its usage or its effect upon hardware or computer
# systems, and maintains copyright as set out in the "LICENCE"
# document which accompanies distributions of Crack v5.0 and upwards.
###

all:
	@echo ""
	@echo "Please don't run 'make' yourself."
	@echo "Invoke 'Crack' to do it for you."
	@echo ""
	@echo "If you don't understand how to do this,"
	@echo "Then read the documentation."
	@echo ""
	@echo "If you still don't understand this,"
	@echo "then go fetch a guru to help."
	@echo ""
	@echo "If the guru doesn't understand,"
	@echo "get a new guru..."
	@echo ""

utils:  library
	@( cd src/util && make $(ARGS) || exit 1 )

library:
	@( cd src/lib && make $(ARGS) || exit 1 )

clean:
	-find . -name "*~" -print | xargs -n50 rm -f
	-( cd src; for dir in * ; do ( cd $$dir ; make clean ) ; done )
#	-make ARGS=clean utils
# skip this last one 'cos "clean" is a simple target caught by the loop

tidy: clean
	-scripts/plaster
	-scripts/fbmerge
	-rm -f run/[DIEGTKM]*
	-rm -f run/dict/gecos.*
	-rm -f run/dict/gcperm.*

spotless: tidy
	-rm -rf run/bin

rmdict:
	-rm -rf run/dict

clobber: spotless rmdict
	-cat run/F-merged >> F-merged.save
	-rm -rf run
	( cd extra ; make clean )
