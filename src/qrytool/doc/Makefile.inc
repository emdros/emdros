XPMFILES = \
         copy.xpm \
         cut.xpm \
         new.xpm \
         open.xpm \
         paste.xpm \
         save.xpm \
         stop.xpm \
         flash.xpm \
         connect.xpm \
         clear.xpm


PNGFILES_FROM_XPM = $(XPMFILES:.xpm=.png)

PNGFILES_STATIC = \
             InputAreaExampleQuery.png \
             InputArea.png MainScreen.png MenuBar.png \
             MyPgConnSettingsFilled.png MyPgSQLConnSettings.png \
             OutputArea.png OutputAreaSampleOutput.png \
             SchemaEnumerations.png \
             SchemaMonadSets.png SchemaObjectTypes.png \
             SQLiteConnSettingsFilled.png SQLiteConnSettings.png \
             ToolBar.png


PNGFILES_ALL = $(PNGFILES_FROM_XPM) $(PNGFILES_STATIC)
