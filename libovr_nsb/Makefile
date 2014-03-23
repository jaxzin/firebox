TARGET = libovr_nsb.so

SOURCES= OVR_Helpers.c OVR_HID.c OVR_Sensor.c

OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))

INCLUDES += -I../gl-matrix.c
CFLAGS = -fPIC -g -Wall -Werror
CC=gcc

all: libovr_nsb.so libovr_nsb.a

.c.o:
	${CC} $(CFLAGS) -c ${INCLUDES} $< 

libovr_nsb.so: $(OBJECTS)
	$(CC) -o libovr_nsb.so $(LDFLAGS) $(OBJECTS) -shared -lpthread

libovr_nsb.a: $(OBJECTS)
	ar -rcs libovr_nsb.a $(OBJECTS) 
		
clean: 
	rm -rf $(OBJECTS) libovr_nsb.so libovr_nsb.a
