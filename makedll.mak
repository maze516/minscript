
!ifdef msc
!include <msc.mak>
!endif

!ifdef linux
!include linux.mak
!endif

!ifdef icc3
!include <icc3.mak>
!endif

!ifdef release
DEBUG = $(DORELEASE)
!else 
DEBUG = $(DODEBUG)
!endif

TARGET	= minextdll
EXTLIB  = proto.obj


build_target:		$(OUTDIR)$(TARGET)$(DLLEXT)

$(OUTDIR)$(TARGET)$(DLLEXT):	$(OUTDIR)$(TARGET)$(OBJEXT)
	$(CC) $(DLLOPT) $(ADDOPT) $(DEBUG) $(DLLOUTOPT)$(OUTDIR)$(TARGET)$(DLLEXT) \
		$(OUTDIR)$(TARGET)$(OBJEXT) $(TARGET).def minipdll.lib $(EXTLIB)
!ifdef icc3
	$(IMPLIB) $(OUTDIR)$(TARGET)$(LIBEXT) $(OUTDIR)$(TARGET)$(DLLEXT)
!endif


$(OUTDIR)$(TARGET)$(OBJEXT):	$(TARGET).cpp
	$(CC) $(DEFINES) $(DEBUG) $(CCOPT) $(CCDLLOPT) $(INCLUDES) $(OUTOPT)$(OUTDIR)$(TARGET)$(OBJEXT) $(TARGET).cpp
