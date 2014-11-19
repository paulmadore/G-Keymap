       lineak_xosdplugin - On Screen Display plugin for the lineakd daemon

Requirements
       The  xosd  plugin requires the following to be added to your configura-
       tion file: Display_plugin = xosd

       The plugin also understands the following config file directives:

       Display_font - This needs to be set to a font with an X style name. The default font is: "-adobe-helvetica-bold-r-normal-*-*-240-*-*-p-*-*-*"
       Display_color - This needs to be set to a numeric color identifier. You can get a color identifier from a program like kcolorchooser. The default color is: "0aff00"
       Display_timeout - This is the amount of time that the display stays on the screen in seconds. The default is 3 seconds.
       Display_pos - This is the horizonal position of the display. Valid values are: top, middle or bottom. The default is bottom.
       Display_align - This is the vertical alignment of the display. Valid values are: left, center or right. The default is center.
       Display_hoffset - This is a number in pixels to offset the horizontal positioning of the display with respect to the Display_pos position. The default is 0.
       Display_voffset - This is a number in pixels to offset the vertical positioning of the display with respect to the Display_align alignment. The default is 50.
       Display_soffset - This is a number in pixels to offset the drop shadowing for the display text. The default is 1.

Known Bugs
       If the font that is set cannot be found, then you can problems upon exit. xosd_close may segfault, causing lineakd to segfault. I am trying to minimize this by falling back to the default font if it cannot be set, but you may still have errors.
