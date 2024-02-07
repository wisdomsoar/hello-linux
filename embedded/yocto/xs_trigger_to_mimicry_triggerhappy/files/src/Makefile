all: xs_trigger

#XS_CM_CFLAGS= -lpthread -ljson-c -lqmiservices -lqmi_encdec -ldsutils -lqmi_cci -lqcmaputils -lqmi_ip -ldsi_netctrl
#XS_CM_CFLAGS+= -DUSE_DSI

XS_TRIGGER_SRC = xs_trigger.c

xs_trigger:
	#$(CC) $(CFLAGS) $(XS_CM_CFLAGS) $(LDFLAGS) $(LIBS) -o xs_cm $(XS_CM_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o xs_trigger $(XS_TRIGGER_SRC)

clean:
	-rm -f *.o
