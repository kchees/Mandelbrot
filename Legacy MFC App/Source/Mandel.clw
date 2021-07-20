; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMandelDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Mandel.h"

ClassCount=4
Class1=CMandelApp
Class2=CMandelDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MANDEL_DIALOG
Class4=CColourSetupDlg
Resource4=IDD_COLOURSETUP_DLG

[CLS:CMandelApp]
Type=0
HeaderFile=Mandel.h
ImplementationFile=Mandel.cpp
Filter=N

[CLS:CMandelDlg]
Type=0
HeaderFile=MandelDlg.h
ImplementationFile=MandelDlg.cpp
Filter=D
LastObject=IDC_AUTO_ZOOM
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=MandelDlg.h
ImplementationFile=MandelDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MANDEL_DIALOG]
Type=1
Class=CMandelDlg
ControlCount=35
Control1=IDOK,button,1342242817
Control2=IDC_OUTPUT,static,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_IMAGE_SIZE,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_REAL_CORNER,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_IMAGINARY_CORNER,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_MAX_ITERATIONS,edit,1350631552
Control11=IDC_CALCULATE,button,1342242816
Control12=IDC_STATIC,static,1342308352
Control13=IDC_SIDE,edit,1350631552
Control14=IDC_LOAD,button,1342242816
Control15=IDC_SAVE,button,1342242816
Control16=IDC_IMAGSIDE_TOP,static,1342308352
Control17=IDC_IMAGSIDE_BOTTOM,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,static,1342308352
Control27=IDC_IMAGSIDE_MID,static,1342308352
Control28=IDC_REALSIDE_LEFT,static,1342308352
Control29=IDC_REALSIDE_MID,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_REALSIDE_RIGHT,static,1342308352
Control32=IDC_STATIC,static,1342308352
Control33=IDC_PROCESSING_TIME,edit,1350633600
Control34=IDC_EXPORT,button,1342242816
Control35=IDC_AUTO_ZOOM,button,1342242819

[DLG:IDD_COLOURSETUP_DLG]
Type=1
Class=CColourSetupDlg
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_MAX_ITERATIONS,static,1342308352
Control5=IDC_COLOUR_SAMPLE,static,1342308356
Control6=IDC_STATIC,static,1342308352
Control7=IDC_RED,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_GREEN,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_BLUE,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_COLOUR_INDEX,combobox,1344339971
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_MIN_ITERATION,edit,1350631552
Control18=IDC_STATIC,static,1342308352
Control19=IDC_MAX_ITERATION,edit,1350631552

[CLS:CColourSetupDlg]
Type=0
HeaderFile=ColourSetupDlg.h
ImplementationFile=ColourSetupDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CColourSetupDlg
VirtualFilter=dWC

