CXX = g++

CXXFLAGS =    -O2 -g -Wall -fmessage-length=0


OBJS =      obj/rm_record.o    \
			obj/rm_rid.o    \
            obj/rm_manager.o \
			obj/rm_filescan.o \
			obj/rm_filehandle.o \
			obj/rm_internal.o 

TARGET =    main
$(TARGET):    $(OBJS)
	$(CXX) -o $@
obj/rm_rid.o: RM/rm_rid.cpp  
	$(CXX) $(CFLAGS) -I. -o $@ -c $<
obj/rm_record.o: RM/rm_record.cpp 
	$(CXX) $(CFLAGS) -I. -o $@ -c $<
obj/rm_filehandle.o: RM/rm_filehandle.cpp 
	$(CXX) $(CFLAGS) -I. -o $@ -c $<
obj/rm_filescan.o: RM/rm_filescan.cpp 
	$(CXX) $(CFLAGS) -I. -o $@ -c $<
obj/rm_manager.o: RM/rm_manager.cpp 
	$(CXX) $(CFLAGS) -I. -o $@ -c $<
obj/rm_internal.o: RM/rm_internal.cpp 
	$(CXX) $(CFLAGS) -I. -o $@ -c $<
	all:	$(TARGET)
	clean:
	　　rm -f $(OBJS) $(TARGET)