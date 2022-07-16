DRIVERS =  systest

include driver/net/net.make

SUBDRIVERS = $(NET)
DRIVERS += $(SUBDRIVERS)