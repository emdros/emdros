XPMFILES = \
           commit.xpm \
           connect.xpm \
           flash.xpm \
           leftarrow.xpm \
           rightarrow.xpm \
           together.xpm 


PNGFILES_FROM_XPM = $(XPMFILES:.xpm=.png)

PNGFILES_COPIED_FROM_QRYTOOL = \
             MyPgConnSettingsFilled.png MyPgSQLConnSettings.png \
             SQLiteConnSettingsFilled.png SQLiteConnSettings.png

PNGFILES_STATIC = \
           MainScreenGreekExample.png MainScreen.png ToolBar.png MenuBar.png


PNGFILES_ALL = $(PNGFILES_FROM_XPM) $(PNGFILES_COPIED_FROM_QRYTOOL) \
               $(PNGFILES_STATIC)

PNGFILES_DISTCLEAN = $(PNGFILES_FROM_XPM) \
                     $(PNGFILES_COPIED_FROM_QRYTOOL)


